module {
  func.func @predict(%arg0: tensor<1x4xf32>) -> tensor<1x2xf32> attributes {iree.module.export} {
    %w = stablehlo.constant dense<[
      [5.000000e-01, -2.500000e-01],
      [2.500000e-01, 7.500000e-01],
      [-5.000000e-01, 1.250000e-01],
      [1.250000e-01, 5.000000e-01]
    ]> : tensor<4x2xf32>
    %b = stablehlo.constant dense<[1.000000e-01, -2.000000e-01]> : tensor<2xf32>
    %dot = stablehlo.dot %arg0, %w : (tensor<1x4xf32>, tensor<4x2xf32>) -> tensor<1x2xf32>
    %bias = stablehlo.broadcast_in_dim %b, dims = [1] : (tensor<2xf32>) -> tensor<1x2xf32>
    %out = stablehlo.add %dot, %bias : tensor<1x2xf32>
    return %out : tensor<1x2xf32>
  }
}
