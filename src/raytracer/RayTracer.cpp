//
// Created by muzio on 8/22/16.
//

#include "raytracer/RayTracer.h"

#include <math.h>

void RayTracer::render(const WorldScene &ws, const Camera& camera, Image* image) {
    Image *imCopy = new Image(image->width, image->height, image->dpi);
    double pixelSizeX = (camera.top_left-camera.top_right).abs()/image->width;
    double pixelSizeY = (camera.top_left-camera.bot_left).abs()/image->height;
    Vector3 cameraHorizontal = (camera.top_right-camera.top_left).normalize();
    Vector3 cameraVertical = (camera.bot_left-camera.top_left).normalize();

    for (int h = 0; h < image->height; h++) {
        for (int w = 0; w < image->width; w++) {

            Vector3 currentPixelPosition = camera.top_left + cameraHorizontal*((w+0.5)*pixelSizeX) + cameraVertical*((h+0.5)*pixelSizeY);
            Ray ray(camera.origin, currentPixelPosition - camera.origin);

            Object *intercObj = NULL;
            double distToInter = ws.getFirstIntersection(ray, intercObj);


            if (distToInter < INF) {
                Vector3 intercPoint = ray.origin + ray.direction * distToInter;
                Vector3 norm = intercObj->getNormalAt(intercPoint);

                for (int i = 0; i < ws.lights().size(); i++) {

                    Light * light =  ws.lights()[i];
                    Vector3 lightDir = light->directionAt(intercPoint);
                    Ray lightRay(intercPoint, lightDir*-1);

                    // diffuse
                    double transmission = 1;
                    for (int j = 0; j < ws.objects().size(); j++) {
                        if (ws.objects()[j] == intercObj) continue;

                        double dist = ws.objects()[j]->getFirstIntersection(lightRay);

                        //if intersects
                        if (dist != INF) {
                            transmission = 0;
                            break;
                        }
                    }
                    double cos = norm.dot(lightDir);
                    if (cos < 0)
                        imCopy->pixels[h][w] = imCopy->pixels[h][w] +
                                light->colorAt(intercPoint) * intercObj->color * transmission * fabs(cos);
                }

            }
        }
    }
    //without post processing
//    image->pixels = imCopy->pixels;

    //post processing
    for (int h = 0; h < image->height; h++) {
        for (int w = 0; w < image->width; w++) {

            Color color(0,0,0);
            int numEle = 0;
            for (int i = -1; i <= 1; i+=2) {
                for (int j = -1; j <= 1; j++) {
                    if (h + i < 0 || h + i >= image->height) continue;
                    if (w + j < 0 || w + j >= image->width) continue;
                    numEle++;
                    color = color + imCopy->pixels[h + i][w + j];
                }
            }
            image->pixels[h][w] = Color(color.r /numEle, color.g / numEle, color.b /numEle);
        }
    }

    image->normalize();
    image->saveToFile("test.bmp", image->dpi);
}

Color RayTracer::traceRay(const Ray &originalRay) {
    return Color();
}
