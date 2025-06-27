//*****************************************************************************
// 定数バッファ
//*****************************************************************************
// マトリクスバッファ
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

// マテリアルバッファ
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte境界用
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ライト用バッファ
struct LIGHT
{
	float4		Direction[10];
	float4		Position[10];
	float4		Diffuse[10];
	float4		Ambient[10];
	float4		Attenuation[10];
	int4		Flags[10];
	int			Enable;
	int			Dummy[3];//16byte境界用
};

cbuffer LightBuffer : register( b4 )
{
	LIGHT		Light;
}

struct FOG
{
	float4		Distance;
	float4		FogColor;
	int			Enable;
	float		Dummy[3];//16byte境界用
};

// フォグ用バッファ
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// リムライト用バッファ
cbuffer Rim : register(b6)
{
	int			rim;
	int			fill[3];
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

// シャドウ用バッファ
cbuffer LightPVM : register(b8)
{
    matrix lightProjectView;
}
// ディゾルブ構造体
struct DISSOLVE
{
    float4 edgeColor;			// エッジの色
    int		Enable;
    float	dissolveThreshold;  // ディゾルブの閾値
    float	edgeWidth;			// エッジの幅
	
    float  dummy[1];			// 16byte境界用
};
// ディゾルブバッファ
cbuffer DissolveBuffer : register(b9)
{
    DISSOLVE Dissolve;
}
// ダメージフィルターバッファ
cbuffer DamageBuffer : register(b10)
{
    float damageAlpha;		// 0.0（無効）～1.0（全画面真っ赤）
    float dummy[3];			// 16バイト境界合わせ
}

//=============================================================================
// 頂点シェーダ
//=============================================================================
void VertexShaderPolygon(in float4 inPosition : POSITION0,
						  in float4 inNormal : NORMAL0,
						  in float4 inDiffuse : COLOR0,
						  in float2 inTexCoord : TEXCOORD0,

						  out float4 outPosition : SV_POSITION,
						  out float4 outNormal : NORMAL0,
						  out float2 outTexCoord : TEXCOORD0,
						  out float4 outDiffuse : COLOR0,
						  out float4 outWorldPos : POSITION0,
						  out float4 outLightPos : TEXCOORD1)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
	outPosition = mul(inPosition, wvp);

	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));
	

	outTexCoord = inTexCoord;

	outWorldPos = mul(inPosition, World);

    outLightPos = mul(outWorldPos, lightProjectView);
	
	outDiffuse = inDiffuse;

}


//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D		g_Texture : register( t0 );
SamplerState	g_SamplerState : register( s0 );

// シャドウマップの追加
Texture2D shadowMap : register(t1);
SamplerComparisonState shadowMapSampler : register(s1);

// ブレンドテクスチャ
Texture2D blendTexture : register(t2);

// 法線マップテクスチャ
Texture2D g_NormalMap : register(t3);
SamplerState g_NormalSampler : register(s3);

// ディゾルブ
Texture2D g_NoiseTexture : register(t4);


//=============================================================================
// ピクセルシェーダ
//=============================================================================
void PixelShaderPolygon( in  float4 inPosition		: SV_POSITION,
						 in  float4 inNormal		: NORMAL0,
						 in  float2 inTexCoord		: TEXCOORD0,
						 in  float4 inDiffuse		: COLOR0,
						 in  float4 inWorldPos      : POSITION0,
						 in  float4 inLightPos		: TEXCOORD1,

						 out float4 outDiffuse		: SV_Target )
{
	float4 color;

	if (Material.noTexSampling == 0)
	{
		color = g_Texture.Sample(g_SamplerState, inTexCoord);

		color *= inDiffuse;
	}
	else
	{
		color = inDiffuse;
	}

	if (Light.Enable == 0)
	{
		color = color * Material.Diffuse;
	}
	else
	{
		float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// ライト数を変更した場合
		for (int i = 0; i < 10; i++)
		{
			float3 lightDir;
			float light;

			if (Light.Flags[i].y == 1)
			{
				if (Light.Flags[i].x == 1)
				{
					lightDir = normalize(Light.Direction[i].xyz);
					light = dot(lightDir, inNormal.xyz);

					light = 0.5 - 0.5 * light;
					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
				}
				else if (Light.Flags[i].x == 2)
				{
					lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
					light = dot(lightDir, inNormal.xyz);

					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

					float distance = length(inWorldPos - Light.Position[i]);

					float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
					tempColor *= att;
				}
				else
				{
					tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
				}

				outColor += tempColor;
			}
		}

		color = outColor;
		color.a = inDiffuse.a * Material.Diffuse.a;
	}

	//フォグ
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}
	
	//縁取り（リムライト）
	if (rim == 1)
	{
		float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
        if (angle > -0.7f)
		{
			//outDiffuse.rb  = 1.0f;
			//outDiffuse.g = 0.0f;	
            outDiffuse.r = 1.0f;
            outDiffuse.gb = 0.0f;
        }
	}
	
	// ディゾルブ
    if (Dissolve.Enable == 1)
    {
        float noiseValue = g_NoiseTexture.Sample(g_SamplerState, inTexCoord).r;
		// ディゾルブの計算
        if (noiseValue < Dissolve.dissolveThreshold)
        {
			// エッジの計算
            if (Dissolve.dissolveThreshold - noiseValue < Dissolve.edgeWidth)
            {
                outDiffuse = Dissolve.edgeColor; //色調整
            }
            else
            {
				// 残る部分は通常のテクスチャ
                discard;
            }
        }
    }

	// シャドウマップ
    if (Light.Enable == 1)
    {
        if (Light.Flags[0].y == 1)
        {
		
            float shadowFactor = 1.0;
            float2 shadowTexCoord = float2(inLightPos.x, -inLightPos.y) / inLightPos.w * 0.5 + 0.5;
            float currentDepth = inLightPos.z / inLightPos.w;
            currentDepth -= 0.005;

            if (shadowTexCoord.x >= 0 && shadowTexCoord.x <= 1 && shadowTexCoord.y >= 0 && shadowTexCoord.y <= 1 && currentDepth >= 0 && currentDepth <= 1)
            {
			// outDiffuse.x = 2.0;
            }
            shadowFactor = shadowMap.SampleCmpLevelZero(shadowMapSampler, shadowTexCoord, currentDepth);

            if (shadowTexCoord.x < 0 || shadowTexCoord.x > 1 || shadowTexCoord.y < 0 || shadowTexCoord.y > 1)
            {
                shadowFactor = 1.0;
            }

            if (shadowFactor == 0)
            {
			//shadowFactor = 0.2;
            }

		// Expereting PCF
			// シャドウの精度
            float2 texelSize = 1.0 / 4096.0; // For each texel size
            float2 offsets[9] =
            {
                float2(-1, -1), float2(0, -1), float2(1, -1),
				float2(-1, 0),  float2(0, 0),  float2(1, 0),
				float2(-1, 1),  float2(0, 1),  float2(1, 1)
            };

            float pcfRadius = 1.0f;
            float shadow = 0.0;
            for (int i = 0; i < 9; ++i)
            {
			// Sampling position
                float2 sampleCoord = shadowTexCoord + offsets[i] * texelSize * pcfRadius;

			// add Sampling result
                shadow += shadowMap.SampleCmpLevelZero(shadowMapSampler, sampleCoord, currentDepth);
            }
		// Get the average value
            shadow /= 9.0;
            if (shadow == 0)
            {
                shadow = 0.4;
            }
            else if (shadow > 0 && shadow < 1)
            {
                shadow = 0.4 + shadow;
                if (shadow > 0.9)
                {
                    shadow = 0.9;
                }
            }

            if (shadowTexCoord.x < 0 || shadowTexCoord.x > 1 || shadowTexCoord.y < 0 || shadowTexCoord.y > 1)
            {
                shadow = 1.0;
            }
		// PCF ending
	
		// outDiffuse.xyz *= shadowFactor;
            outDiffuse.xyz *= shadow;
        }
    }
	
	// ダメージフィルター
	
    outDiffuse = lerp(outDiffuse, float4(1, 0, 0, outDiffuse.a), damageAlpha);
}

//=============================================================================
// フルスクリーントライアングル頂点シェーダ
//=============================================================================
void VS_FullScreen(	uint vid		: SV_VertexID,
					out float4 pos	: SV_POSITION)
{
    // SV_VertexID = 0,1,2 だけを使い、画面全体を覆う三角形を生成
    float2 pts[3] =
    {
        float2(-1, -1),
        float2(-1, 3),
        float2(3, -1)
    };
    pos = float4(pts[vid], 0, 1);
}
//=============================================================================
// ダメージオーバーレイピクセルシェーダー
//=============================================================================
float4 PS_FullScreen() : SV_Target
{
    // 既に画面に描かれたピクセル（バックバッファ）にはアクセスできないため
    // BlendState(ALPHABLEND) ですり抜けさせつつ、赤を重ねる
    return float4(1, 0, 0, damageAlpha);
}