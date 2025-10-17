#include "Object3d.hlsli"

struct Material {
    float4 color;
    int enableLighring;
    float4x4 uvMatrix;
    float shininess;
};

//平行光源
struct DirectionalLight {
    float4 color; //ライトの色
    float3 direction; //ライトの向き
    float intensity; //輝度
    int isLambert; //lambertにするかどうか
    int isBlinnPhong; //BlinnPhongReflectionを行うかどうか
};

//点光源
struct PointLight {
    float4 color;//ライトの色
    float3 position;//ライトの位置
    float intensity;//輝度
};

//カメラ
struct Camera {
    float3 worldPosition;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
//ConstantBuffer<PointLight> gPointLight : register(b3);

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvMatrix);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    PixelShaderOutput output;
    
    //Lightingする場合
    if (gMaterial.enableLighring) {
        //コサイン
        float cos = 0.0f;
        
        //ライティングの方法
        if (gDirectionalLight.isLambert) {
            //lambert
            cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        } else {
            //half lambert
            float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
            //拡散反射
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        }
        
        
        //カメラの視点を取得
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        
        //鏡面反射の強度
        float specularPow = 0.0f;
        if (gDirectionalLight.isBlinnPhong) {
            //BlingPhongReflectionModel
            //ハーフベクトル
            float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
            float NDotH = dot(normalize(input.normal), halfVector);
            specularPow = pow(saturate(NDotH), gMaterial.shininess); //反射強度
        } else {
            //PhongReflectionModel
            //入射光の反射ベクトルを求める
            float3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
            //鏡面反射の強度
            float RdotE = dot(reflectLight, toEye);
            specularPow = pow(saturate(RdotE), gMaterial.shininess); //反射強度
        }
        
        
        //NdotL = step(0.25f, NdotL);
        float3 diffeuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        
        //鏡面反射
        float3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        
        //拡散反射+鏡面反射
        output.color.rgb = diffeuse + specular;
        //アルファ値
        output.color.a = gMaterial.color.a * textureColor.a;
    } else {
        output.color = gMaterial.color * textureColor;
    }
    //textureのα値が0.5f以下の時にPixelを棄却
    if (textureColor.a <= 0.5) {
        discard;
    }
    //textureのα値の0の時にPixelを棄却
    if (textureColor.a == 0.0) {
        discard;
    }
    //output.colorのα値が0の時にPixelを棄却
    if (output.color.a == 0.0) {
        discard;
    }
    return output;
}