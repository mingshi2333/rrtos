module {
  func.func @main(%arg0: tensor<4xf32>) -> tensor<4xf32> attributes {iree.module.export} {
    %0 = arith.addf %arg0, %arg0 : tensor<4xf32>
    return %0 : tensor<4xf32>
  }
}
