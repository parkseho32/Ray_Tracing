#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "Hittable.h"

class Camera
{
public:
    double aspectRatio = 1.0;  //이미지의 가로 세로 비율
    int imageWidth = 100;      //렌더링된 이미지의 픽셀 너비 

    void Render(const Hittable& world)
    {
        Initialize();

        std::cout << "P3\n" << imageWidth << ' ' << mImageHeight << "\n255\n";

        for (int scanlineIndex = 0; scanlineIndex < mImageHeight; scanlineIndex++)
        {
            std::clog
                << "\rScanlines remaining: "
                << (mImageHeight - scanlineIndex)
                << ' '
                << std::flush;

            for (int pixelIndex = 0; pixelIndex < imageWidth; pixelIndex++)
            {
                Color pixelColor(0.0, 0.0, 0.0);

                for (int sampleIndex = 0; sampleIndex < samplesPerPixel; sampleIndex++)
                {
                    Ray ray = GetRay(pixelIndex, scanlineIndex);
                    pixelColor += RayColor(ray, world);
                }

                WriteColor(std::cout, mPixelSamplesScale * pixelColor);
            }
        }

        std::clog << "\rDone.                 \n";
    }

private:
    void Initialize()
    {
        mImageHeight = static_cast<int>(imageWidth / aspectRatio);
        mImageHeight = (mImageHeight < 1) ? 1 : mImageHeight;

        mPixelSamplesScale = 1.0 / static_cast<double>(samplesPerPixel);

        mCenter = Point3(0.0, 0.0, 0.0);

        // 뷰포트 크기 결정하기
        auto focalLength = 1.0;
        auto viewportHeight = 2.0;
        auto viewportWidth = viewportHeight * (static_cast<double>(imageWidth) / mImageHeight);

        // 뷰포트의 수평 및 수직 가장자리를 따라 벡터를 계산합니다.
        auto viewportU = Vec3(viewportWidth, 0.0, 0.0);
        auto viewportV = Vec3(0.0, -viewportHeight, 0.0);

        // 픽셀 간 수평 및 수직 델타 벡터를 계산합니다
        mPixelDeltaU = viewportU / imageWidth;
        mPixelDeltaV = viewportV / mImageHeight;

        // 왼쪽 위 픽셀의 위치 계산
        auto viewportUpperLeft =
            mCenter
            - Vec3(0.0, 0.0, focalLength)
            - viewportU / 2.0
            - viewportV / 2.0;

        mPixel00Location = viewportUpperLeft + 0.5 * (mPixelDeltaU + mPixelDeltaV);
    }

    Ray GetRay(int pixelIndex, int scanlineIndex) const
    {
        auto offset = SampleSquare();

        auto pixelSample =
            mPixel00Location
            + ((pixelIndex + offset.X()) * mPixelDeltaU)
            + ((scanlineIndex + offset.Y()) * mPixelDeltaV);

        auto rayOrigin = mCenter;
        auto rayDirection = pixelSample - rayOrigin;

        return Ray(rayOrigin, rayDirection);
    }

    Vec3 SampleSquare() const
    {
        return Vec3(RandomDouble() - 0.5, RandomDouble() - 0.5, 0.0);
    }

    Color RayColor(const Ray& ray, const Hittable& world) const
    {
        HitRecord hitRecord;

        if (world.Hit(ray, Interval(0.0, Infinity), hitRecord))
        {
            return 0.5 * (hitRecord.Normal + Color(1.0, 1.0, 1.0));
        }

        Vec3 unitDirection = UnitVector(ray.Direction());
        auto a = 0.5 * (unitDirection.Y() + 1.0);

        return (1.0 - a) * Color(1.0, 1.0, 1.0)
            + a * Color(0.5, 0.7, 1.0);
    }

private:
    int samplesPerPixel = 10;

    int mImageHeight = 0;        // 렌더링된 이미지 높이
    double mPixelSamplesScale = 1.0;

    Point3 mCenter;               // 카메라 센터
    Point3 mPixel00Location;      // 픽셀 위치 0, 0
    Vec3 mPixelDeltaU;           // 오른쪽으로 픽셀 오프셋 적용
    Vec3 mPixelDeltaV;           // 아래쪽 픽셀로 오프셋
};
#endif