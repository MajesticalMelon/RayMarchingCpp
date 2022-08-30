#version 330

uniform sampler2D tex;

const int MAX_ITERS = 50;
const float SUBPIXEL_QUALITY = 0.75;
uniform vec2 windowDimensions = vec2 (800, 600);
vec2 inverseDimensions = 1. / windowDimensions;

out vec4 fragColor;
vec2 uv = gl_FragCoord.xy / windowDimensions;

// Simon Rodriguez implementation of FXAA

const float EDGE_THRESHOLD_MIN = 0.0312;
const float EDGE_THRESHOLD_MAX = 0.125;

float rgb2luma(vec3 rgb) {
	return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

float QUALITY(int iter) {
	return max(1., iter * 0.5 - 1);
}

void main() {
	fragColor.a = 1;

	vec3 colorCenter = texture(tex, uv).rgb;

	// Calculate luma at current and surrounding pixels
	float lumaCenter = rgb2luma(colorCenter);
	
	float lumaDown = rgb2luma(textureOffset(tex, uv, ivec2(0, -1)).rgb);
	float lumaUp = rgb2luma(textureOffset(tex, uv, ivec2(0, 1)).rgb);
	float lumaLeft = rgb2luma(textureOffset(tex, uv, ivec2(-1, 0)).rgb);
	float lumaRight = rgb2luma(textureOffset(tex, uv, ivec2(1, 0)).rgb);
	
	float lumaMin  = min(lumaCenter, min(min(lumaDown, lumaUp), min(lumaLeft, lumaRight)));
	float lumaMax = max(lumaCenter, max(max(lumaDown, lumaUp), max(lumaLeft, lumaRight)));
	
	float lumaRange = lumaMax - lumaMin;

	if (lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD_MAX)) {
		fragColor.rgb = colorCenter;
		return;
	}

	float lumaDownLeft = rgb2luma(textureOffset(tex, uv, ivec2(-1, -1)).rgb);
	float lumaUpRight = rgb2luma(textureOffset(tex, uv, ivec2(1, 1)).rgb);
	float lumaUpLeft = rgb2luma(textureOffset(tex, uv, ivec2(-1, 1)).rgb);
	float lumaDownRight = rgb2luma(textureOffset(tex, uv, ivec2(1, -1)).rgb);

	float lumaDownUp = lumaDown + lumaUp;
	float lumaLeftRight = lumaLeft + lumaRight;

	float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
	float lumaDownCorners = lumaDownLeft + lumaDownRight;
	float lumaUpCorners = lumaUpLeft + lumaUpRight;
	float lumaRightCorners = lumaDownRight + lumaUpRight;

	float edgeHorizontal = abs(-2. * lumaLeft + lumaLeftCorners) + abs(-2. * lumaCenter + lumaDownUp) * 2. + abs(-2. * lumaRight + lumaRightCorners);
	float edgeVertical = abs(-2. * lumaUp + lumaUpCorners) + abs(-2. * lumaCenter + lumaLeftRight) * 2. + abs(-2. * lumaDown + lumaDownCorners);

	bool isHorizontal = (edgeHorizontal >= edgeVertical);

	float luma1 = isHorizontal ? lumaDown : lumaLeft;
	float luma2 = isHorizontal ? lumaUp : lumaRight;

	float gradient1 = luma1 - lumaCenter;
	float gradient2 = luma2 - lumaCenter;

	bool is1Steepest = abs(gradient1) >= abs(gradient2);
	
	float gradientScaled = 0.25 * max(abs(gradient1), abs(gradient2));

	float stepLength = isHorizontal ? inverseDimensions.y : inverseDimensions.x;

	float lumaLocalAverage = 0.;

	if (is1Steepest) {
		stepLength = -stepLength;
		lumaLocalAverage = 0.5 * (luma1 + lumaCenter);
	} else {
		lumaLocalAverage = 0.5 * (luma2 + lumaCenter);
	}

	vec2 currentUV = uv;
	if (isHorizontal) {
		currentUV.y += stepLength * 0.5;
	} else {
		currentUV.x += stepLength * 0.5;
	}

	vec2 offset = isHorizontal ? vec2(inverseDimensions.x, 0.) : vec2(0., inverseDimensions.y);
	
	vec2 uv1 = currentUV - offset;
	vec2 uv2 = currentUV + offset;

	float lumaEnd1 = rgb2luma(texture(tex, uv1).rgb);
	float lumaEnd2 = rgb2luma(texture(tex, uv2).rgb);
	lumaEnd1 -= lumaLocalAverage;
	lumaEnd2 -= lumaLocalAverage;

	bool reached1 = abs(lumaEnd1) >= gradientScaled;
	bool reached2 = abs(lumaEnd2) >= gradientScaled;
	bool reachedBoth = reached1 && reached2;

	if (!reached1) {
		uv1 -= offset;
	}

	if (!reached2) {
		uv2 += offset;
	}

	if (!reachedBoth) {
		for (int i = 2; i < MAX_ITERS; i++) {
			if (!reached1) {
				lumaEnd1 = rgb2luma(texture(tex, uv1).rgb);
				lumaEnd1 = lumaEnd1 - lumaLocalAverage;
			}

			if (!reached2) {
				lumaEnd2 = rgb2luma(texture(tex, uv2).rgb);
				lumaEnd2 = lumaEnd2 - lumaLocalAverage;
			}

			reached1 = abs(lumaEnd1) >= gradientScaled;
			reached2 = abs(lumaEnd2) >= gradientScaled;
			reachedBoth = reached1 && reached2;

			if (!reached1) {
				uv1 -= offset * QUALITY(i);
			}

			if (!reached2) {
				uv2 += offset * QUALITY(i);
			}

			if (reachedBoth) break;
		}
	}

	float distance1 = isHorizontal ? (uv.x - uv1.x) : (uv.y - uv1.y);
	float distance2 = isHorizontal ? (uv.x - uv2.x) : (uv.y - uv2.y);

	bool isDirectional = distance1 < distance2;
	float distanceFinal = min(distance1, distance2);

	float edgeThickness = distance1 + distance2;
	
	float pixelOffset = -distanceFinal / edgeThickness + 0.5;

	bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;

	bool correctVariation = ((isDirectional ? lumaEnd1: lumaEnd2) < 0.) != isLumaCenterSmaller;

	float finalOffset = correctVariation ? pixelOffset : 0.;

	float lumaAverage = (1. / 12.) * (2. * (lumaDownUp + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);

	float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter) / lumaRange, 0., 1.);
	float subPixelOffset2 = (-2. * subPixelOffset1 + 3.) * subPixelOffset1 * subPixelOffset1;

	float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * SUBPIXEL_QUALITY;

	vec2 finalUV = uv;

	if (isHorizontal) {
		finalUV.y += finalOffset * stepLength;
	} else {
		finalUV.x += finalOffset * stepLength;
	}
	
	vec3 finalColor = texture(tex, finalUV).rgb;
	fragColor = vec4(finalColor, 1.);
}