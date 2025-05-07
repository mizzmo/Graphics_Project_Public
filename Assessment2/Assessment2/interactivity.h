#pragma once

glm::vec3 screenToWorldRay(double xpos, double ypos, int width, int height, glm::mat4 projection, glm::mat4 view) {
	// Converts screen click to world-space ray
	float x = (2.0f * xpos) / width - 1.0f;
	float y = 1.0f - (2.0f * ypos) / height;
	glm::vec4 ray_clip(x, y, -1.0, 1.0);

	glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	glm::vec3 ray_world = glm::normalize(glm::vec3(glm::inverse(view) * ray_eye));
	return ray_world;
}


bool rayIntersectsSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& center, float radius) {
	glm::vec3 oc = rayOrigin - center;
	float b = glm::dot(oc, rayDir);
	float c = glm::dot(oc, oc) - radius * radius;
	float h = b * b - c;
	return h >= 0.0f;
}