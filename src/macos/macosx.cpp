#include "cut-Bridging-Header.h"
#include <Scene.h>
#include <sys/_types/_int32_t.h>

static Scene *scene = nullptr;

void cpp_init(int32_t width, int32_t height) {
    scene = new Scene();
    scene->init(width, height);
}

void cpp_update() {
    scene->update();
}

void cpp_draw() {
    scene->render();
}

void cpp_terminate() {
    delete scene;
}