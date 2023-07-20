# Fake-Through-Scope

一个辐射4的MOD，主要采用了Hook D3D11的方式，理论上只要能获取到足够数据并且能够注入dll的DX11游戏都能用。

在DrawIndexed阶段对特定纹理进行操作，保存所需要的数据，在TAA之后绘制，实现2D但是能够3D跟随和形变的伪3D画中画效果。

画中画的效果是对BackBuffer截取并修改而成，不是双摄像机。

已知问题：
1. 在辐射4中，需要一个实现了proxy的dxgi.dll或者d3d11.dll才能正常使用（DrawIndexed在缺少了这个前置条件下无法勾取）
2. 在使用CommonLibF4获取FOV时，诸如FOVAdjust，FOVAdjustTarget等数据获取不太稳定，因此放弃了多FOV的兼容。
