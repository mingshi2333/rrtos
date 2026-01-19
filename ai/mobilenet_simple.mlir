// Simplified MobileNet-like model for embedded testing
module {
  // Simple inference: vector operation simulating image classification
  func.func @mobilenet_infer(%input: tensor<1000xf32>) -> tensor<10xf32> attributes {iree.module.export} {
    // Simulate 10-class classification from 1000 features
    %c0 = arith.constant 0.0 : f32
    %init = tensor.empty() : tensor<10xf32>
    %filled = linalg.fill ins(%c0 : f32) outs(%init : tensor<10xf32>) -> tensor<10xf32>
    
    // Simple element-wise operation
    %result = arith.mulf %input, %input : tensor<1000xf32>
    
    // Extract first 10 elements (simplified)
    %slice = tensor.extract_slice %result[0] [10] [1] : tensor<1000xf32> to tensor<10xf32>
    
    return %slice : tensor<10xf32>
  }
}
