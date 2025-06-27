//*****************************************************************************
// �萔�o�b�t�@
//*****************************************************************************
// �}�g���N�X�o�b�t�@
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

// �}�e���A���o�b�t�@
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte���E�p
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ���C�g�p�o�b�t�@
struct LIGHT
{
	float4		Direction[10];
	float4		Position[10];
	float4		Diffuse[10];
	float4		Ambient[10];
	float4		Attenuation[10];
	int4		Flags[10];
	int			Enable;
	int			Dummy[3];//16byte���E�p
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
	float		Dummy[3];//16byte���E�p
};

// �t�H�O�p�o�b�t�@
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// �������C�g�p�o�b�t�@
cbuffer Rim : register(b6)
{
	int			rim;
	int			fill[3];
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

// �V���h�E�p�o�b�t�@
cbuffer LightPVM : register(b8)
{
    matrix lightProjectView;
}
// �f�B�]���u�\����
struct DISSOLVE
{
    float4 edgeColor;			// �G�b�W�̐F
    int		Enable;
    float	dissolveThreshold;  // �f�B�]���u��臒l
    float	edgeWidth;			// �G�b�W�̕�
	
    float  dummy[1];			// 16byte���E�p
};
// �f�B�]���u�o�b�t�@
cbuffer DissolveBuffer : register(b9)
{
    DISSOLVE Dissolve;
}
// �_���[�W�t�B���^�[�o�b�t�@
cbuffer DamageBuffer : register(b10)
{
    float damageAlpha;		// 0.0�i�����j�`1.0�i�S��ʐ^���ԁj
    float dummy[3];			// 16�o�C�g���E���킹
}

//=============================================================================
// ���_�V�F�[�_
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
// �O���[�o���ϐ�
//*****************************************************************************
Texture2D		g_Texture : register( t0 );
SamplerState	g_SamplerState : register( s0 );

// �V���h�E�}�b�v�̒ǉ�
Texture2D shadowMap : register(t1);
SamplerComparisonState shadowMapSampler : register(s1);

// �u�����h�e�N�X�`��
Texture2D blendTexture : register(t2);

// �@���}�b�v�e�N�X�`��
Texture2D g_NormalMap : register(t3);
SamplerState g_NormalSampler : register(s3);

// �f�B�]���u
Texture2D g_NoiseTexture : register(t4);


//=============================================================================
// �s�N�Z���V�F�[�_
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

		// ���C�g����ύX�����ꍇ
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

	//�t�H�O
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
	
	//�����i�������C�g�j
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
	
	// �f�B�]���u
    if (Dissolve.Enable == 1)
    {
        float noiseValue = g_NoiseTexture.Sample(g_SamplerState, inTexCoord).r;
		// �f�B�]���u�̌v�Z
        if (noiseValue < Dissolve.dissolveThreshold)
        {
			// �G�b�W�̌v�Z
            if (Dissolve.dissolveThreshold - noiseValue < Dissolve.edgeWidth)
            {
                outDiffuse = Dissolve.edgeColor; //�F����
            }
            else
            {
				// �c�镔���͒ʏ�̃e�N�X�`��
                discard;
            }
        }
    }

	// �V���h�E�}�b�v
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
			// �V���h�E�̐��x
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
	
	// �_���[�W�t�B���^�[
	
    outDiffuse = lerp(outDiffuse, float4(1, 0, 0, outDiffuse.a), damageAlpha);
}

//=============================================================================
// �t���X�N���[���g���C�A���O�����_�V�F�[�_
//=============================================================================
void VS_FullScreen(	uint vid		: SV_VertexID,
					out float4 pos	: SV_POSITION)
{
    // SV_VertexID = 0,1,2 �������g���A��ʑS�̂𕢂��O�p�`�𐶐�
    float2 pts[3] =
    {
        float2(-1, -1),
        float2(-1, 3),
        float2(3, -1)
    };
    pos = float4(pts[vid], 0, 1);
}
//=============================================================================
// �_���[�W�I�[�o�[���C�s�N�Z���V�F�[�_�[
//=============================================================================
float4 PS_FullScreen() : SV_Target
{
    // ���ɉ�ʂɕ`���ꂽ�s�N�Z���i�o�b�N�o�b�t�@�j�ɂ̓A�N�Z�X�ł��Ȃ�����
    // BlendState(ALPHABLEND) �ł��蔲�������A�Ԃ��d�˂�
    return float4(1, 0, 0, damageAlpha);
}