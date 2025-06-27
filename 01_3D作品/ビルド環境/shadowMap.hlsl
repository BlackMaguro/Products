
//*****************************************************************************
// 定数バッファ
//*****************************************************************************

cbuffer WorldBuffer : register( b0 )
{
	matrix World;
}

cbuffer ViewBuffer : register( b1 )
{
	matrix View;
}

cbuffer ProjectionBuffer : register( b2 )
{
	matrix Projection;
}


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}


cbuffer LightPVM : register(b8)
{
    matrix lightProjectView;
}


//=============================================================================
// 頂点シェーダ
//=============================================================================
void VertexShaderShadowMap(
    in  float4 inPosition		: POSITION0,
    in  float4 inNormal		    : NORMAL0,
    in  float4 inDiffuse		: COLOR0,
    in  float2 inTexCoord		: TEXCOORD0,

    out float4 outPosition	    : SV_POSITION,
	out float2 outTexCoord		: TEXCOORD
	)
{
	outPosition = mul (inPosition , mul(World,lightProjectView));

	outTexCoord = inTexCoord;

}


//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D 		shadowMap   : register ( t1 );
SamplerComparisonState shadowMapSampler  : register ( s1 );