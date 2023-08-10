# Fake-Through-Scope

一个辐射4的MOD，主要采用了Hook D3D11的方式，理论上只要能获取到足够数据并且能够注入dll的DX11游戏都能用。

在DrawIndexed阶段对特定纹理进行操作，保存所需要的数据，在TAA之后绘制，实现2D但是能够3D跟随和形变的伪3D画中画效果。

画中画的效果是对BackBuffer截取并修改而成，不是双摄像机。

已知问题：
1. 在辐射4中，需要一个实现了proxy的dxgi.dll或者d3d11.dll才能正常使用（DrawIndexed在缺少了这个前置条件下无法勾取）


A MOD of Fallout 4, mainly using Hook D3D, theoretically as long as can get enough data and can be injected into the dll DX11 game can be used.

In the DrawIndexed stage, the specific texture is operated, the required data is saved, and the pseudo-3D picture-in-picture effect with 2D but 3D following and deformation is achieved after the TAA.

The picture-in-picture effect is a BackBuffer cut and modified instead of using two cameras.

Known issues:

1. In Fallout 4, you need a dxgi.dll or d3d11.dll that implements proxy to work properly (DrawIndexed cannot be invoked without this precondition).
