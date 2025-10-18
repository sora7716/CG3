#include "Object3d.hlsli"

//マテリアル
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
    float4 color; //ライトの色
    float3 position; //ライトの位置
    float intensity; //輝度
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
ConstantBuffer<PointLight> gPointLight : register(b3);

//ライティング
struct Lighting {
    float3 diffuse;
    float3 specular;
};

//平行光源
Lighting DirectionalLighting(VertexShaderOutput input, float4 textureColor, float3 toEye) {
    //コサイン
    float NDotDirectional = 0.0f;
        
    //ライティングの方法
    if (gDirectionalLight.isLambert) {
        //lambert
        NDotDirectional = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
    } else {
        //half lambert
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        //拡散反射
        NDotDirectional = pow(NdotL * 0.5f + 0.5f, 2.0f);
    }
        
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
    
    //ライティング
    Lighting result;
    
    //NdotL = step(0.25f, NdotL);
    result.diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NDotDirectional * gDirectionalLight.intensity;
        
    //鏡面反射
    result.specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);

    return result;
}

//点光源
Lighting PointLighing(VertexShaderOutput input, float4 textureColor, float3 toEye) {
    //BlingPhongReflectionModel
    //点光源の方向ベクトル
    float3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
    //ハーフベクトル
    float3 halfVector = normalize(-pointLightDirection + toEye);
    float NDotH = dot(normalize(input.normal), halfVector);
    //反射強度
    float specularPowForPoint = pow(saturate(NDotH), gMaterial.shininess); 
    
    //コサイン
    float NDotPoint = saturate(dot(normalize(input.normal), -pointLightDirection));
    
    //距離減衰
    float distance = length(gPointLight.position - input.worldPosition);
    float attenuation = 1.0f / pow(distance, 2.0f);
    
    //ライティング
    Lighting result;
    //拡散反射
    result.diffuse = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * NDotPoint * gPointLight.intensity;
    //鏡面反射
    result.specular= gPointLight.color.rgb * gPointLight.intensity * specularPowForPoint * float3(1.0f, 1.0f, 1.0f);
    
    //距離減衰を適応
    result.diffuse *= attenuation;
    result.specular *= attenuation;
    return result;
}

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvMatrix);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    PixelShaderOutput output;
    
    //Lightingする場合
    if (gMaterial.enableLighring) {
        //カメラの視点を取得
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
     
        //平行光源
        Lighting directionalLighting = DirectionalLighting(input, textureColor, toEye);
        
        //点光源
        Lighting pointLighting = PointLighing(input, textureColor, toEye);
        
        //拡散反射+鏡面反射
        output.color.rgb = directionalLighting.diffuse + directionalLighting.specular + pointLighting.diffuse + pointLighting.specular;
        //アルファ  
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