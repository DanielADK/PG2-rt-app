//
// Created by Daniel Adámek - daniel@nullptr.cz on 9.5.25.
// Copyright (c) 2025 FM TUL. All rights reserved.
//

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <opencv2/opencv.hpp>
#include <GL/glew.h>
#include <filesystem>

// generate GL texture from image file
GLuint textureInit(const std::filesystem::path& file_name);

// generate GL texture from OpenCV image
GLuint gen_tex(cv::Mat& image);

#endif