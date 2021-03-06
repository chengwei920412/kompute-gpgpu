
#include <iostream>
#include <memory>
#include <vector>

#include "kompute/Kompute.hpp"

int main()
{
    kp::Manager mgr;

    auto tensorInA = mgr.tensor<float>({ 2.0, 4.0, 6.0 });
    auto tensorInB = mgr.tensor<float>({ 0.0, 1.0, 2.0 });
    auto tensorOut = mgr.tensor<float>({ 0.0, 0.0, 0.0 });

    std::string shader(R"(
        // The version to use 
        #version 450

        // The execution structure
        layout (local_size_x = 1) in;

        // The buffers are provided via the tensors
        layout(binding = 0) buffer bufA { float a[]; };
        layout(binding = 1) buffer bufB { float b[]; };
        layout(binding = 2) buffer bufOut { float o[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;

            o[index] = a[index] * b[index];
        }
      )");

    std::vector<std::shared_ptr<kp::Tensor>> params = { tensorInA, tensorInB, tensorOut };

    std::shared_ptr<kp::Algorithm> algo = mgr.algorithm(params, kp::Shader::compileSource(shader));

    mgr.sequence()
        ->record<kp::OpTensorSyncDevice>(params)
        ->record<kp::OpAlgoDispatch>(algo)
        ->record<kp::OpTensorSyncLocal>(params);

    // prints "Output {  0  4  12  }"
    std::cout<< "Output: {  ";
    for (const float& elem : tensorOut->data()) {
      std::cout << elem << "  ";
    }
    std::cout << "}" << std::endl;
}

