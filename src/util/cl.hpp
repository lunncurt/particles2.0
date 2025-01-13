#pragma once

#define CL_HPP_TARGET_OPENCL_VERSION 300

#include <CL/opencl.hpp>
#include "part.hpp"
#include <iostream>
#include <vector>

const std::string kernel_code = R"CLC(
kernel void particle(
  const float mass, 
  const int size,
  global float *xposition,
  global float *yposition,
  global float *xvelocity,
  global float *yvelocity)
{
  size_t i = get_global_id(0);

  float2 totalforce;
  totalforce.x = 0;
  totalforce.y = 0;
  
  for(size_t j = 0; j < size; j++){
    if (i == j) continue;

    float2 displacement;

    displacement.x = xposition[j] - xposition[i];
    displacement.y = yposition[j] - yposition[i];

    const float distance =
      displacement.x * displacement.x + displacement.y * displacement.y + 10.f;

    const float g = 6.67430e-11;

    const float fmag = g * mass * mass / distance;

    totalforce += displacement / sqrt(distance) * fmag;
  }


  xvelocity[i] += totalforce.x / mass;
  yvelocity[i] += totalforce.y / mass;

  const float scale = 0.0005f;

  xposition[i] += xvelocity[i] * scale;
  yposition[i] += yvelocity[i] * scale;
}
)CLC";

class CLUtil {
private:
  cl::Context context;
  cl::CommandQueue q;
  cl::Program program;
  cl::Kernel kernel;

public:
  CLUtil() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    std::vector<cl::Device> devices;
    platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devices);

    context = cl::Context({devices[0]});

    program = cl::Program(context, kernel_code);

    if (program.build({devices[0]}) != CL_SUCCESS) {
      std::cout << " Error building: "
                << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0])
                << "\n";
      exit(1);
    }

    q = cl::CommandQueue(context, devices[0]);
  }

  void runner(Particle &p){
    cl::Buffer buffer_MASS(context, CL_MEM_READ_ONLY, sizeof(float));
    cl::Buffer buffer_SIZE(context, CL_MEM_READ_ONLY, sizeof(int));
    cl::Buffer buffer_XPOS(context, CL_MEM_READ_WRITE, sizeof(float) * p.size);
    cl::Buffer buffer_YPOS(context, CL_MEM_READ_WRITE, sizeof(float) * p.size);
    cl::Buffer buffer_XVELO(context, CL_MEM_READ_WRITE, sizeof(float) * p.size);
    cl::Buffer buffer_YVELO(context, CL_MEM_READ_WRITE, sizeof(float) * p.size);

    q.enqueueWriteBuffer(buffer_XPOS, CL_TRUE, 0, sizeof(float) * p.size, p.xpos);
    q.enqueueWriteBuffer(buffer_YPOS, CL_TRUE, 0, sizeof(float) * p.size, p.ypos);
    q.enqueueWriteBuffer(buffer_XVELO, CL_TRUE, 0, sizeof(float) * p.size, p.xvelocity);
    q.enqueueWriteBuffer(buffer_YVELO, CL_TRUE, 0, sizeof(float) * p.size, p.yvelocity);

    // run the kernel
    cl::Kernel kernel_particle(program, "particle");

    kernel_particle.setArg(0, p.mass);
    kernel_particle.setArg(1, p.size);
    kernel_particle.setArg(2, buffer_XPOS);
    kernel_particle.setArg(3, buffer_YPOS);
    kernel_particle.setArg(4, buffer_XVELO);
    kernel_particle.setArg(5, buffer_YVELO);

    q.enqueueNDRangeKernel(kernel_particle, cl::NullRange, cl::NDRange(p.size), cl::NullRange);
    q.finish();

    q.enqueueReadBuffer(buffer_XPOS, CL_TRUE, 0, sizeof(float) * p.size, p.xpos);
    q.enqueueReadBuffer(buffer_YPOS, CL_TRUE, 0, sizeof(float) * p.size, p.ypos);
    q.enqueueReadBuffer(buffer_XVELO, CL_TRUE, 0, sizeof(float) * p.size, p.xvelocity);
    q.enqueueReadBuffer(buffer_YVELO, CL_TRUE, 0, sizeof(float) * p.size, p.yvelocity);
  }
};
