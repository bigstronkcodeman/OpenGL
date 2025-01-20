//
// Created by prbou on 1/13/2025.
//

#ifndef OPENGL_FUN_TIMERS_H
#define OPENGL_FUN_TIMERS_H

#include <glad/glad.h>
#include <iostream>
#include <string>

class OpenGLTimer {
private:
    GLuint queryID{};
    std::string name;

public:
    OpenGLTimer(const std::string& name) : name(name) {
        glGenQueries(1, &queryID);
        glBeginQuery(GL_TIME_ELAPSED, queryID);
    }

    ~OpenGLTimer() {
        glEndQuery(GL_TIME_ELAPSED);

        GLuint64 elapsedTime = 0;
        glGetQueryObjectui64v(queryID, GL_QUERY_RESULT, &elapsedTime);

        double elapsedTimeMs = elapsedTime / 1e6;
        std::cout << "OpenGL Timer: Elapsed time for " << name << " = " << elapsedTimeMs << " ms\n";

        glDeleteQueries(1, &queryID);
    }
};

#include <glad/glad.h>
#include <chrono>
#include <iostream>

class Timer {
public:
    Timer(const std::string& name = "Timer") : timerName(name) {
        cpuStart = std::chrono::high_resolution_clock::now();

        glGenQueries(1, &gpuQueryID);
        glBeginQuery(GL_TIME_ELAPSED, gpuQueryID);
    }

    ~Timer() {
        glEndQuery(GL_TIME_ELAPSED);

        auto cpuEnd = std::chrono::high_resolution_clock::now();

        GLuint64 gpuElapsedTime = 0;
        glGetQueryObjectui64v(gpuQueryID, GL_QUERY_RESULT, &gpuElapsedTime);

        double gpuTimeMs = gpuElapsedTime / 1e6;

        double cpuTimeMs = std::chrono::duration<double, std::milli>(cpuEnd - cpuStart).count();

        std::cout << "[" << timerName << "] CPU Time: " << cpuTimeMs << " ms | GPU Time: " << gpuTimeMs << " ms\n";

        glDeleteQueries(1, &gpuQueryID);
    }

private:
    GLuint gpuQueryID;
    std::chrono::high_resolution_clock::time_point cpuStart;
    std::string timerName;
};



#endif //OPENGL_FUN_TIMERS_H
