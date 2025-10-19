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
    int enableDirectonalLighting; //平行光源を有効にするか
};

//点光源
struct PointLight {
    float4 color; //ライトの色
    float3 position; //ライトの位置
    float intensity; //輝度
    int isBlinnPhong; //BlinnPhongReflectionを行うかどうか
    int enablePointLighting; //点光源を有効にするか
};

//スポットライト
struct SpotLight {
    float4 color; //ライトの色
    float3 position; //ライトの位置
    float intensity; //輝度
    float3 direction; //スポットライトの方向
    float distance; //ライトの届く最大距離
    float decay; //減衰率
    float cosAngle; //スポットライトの余弦
    float cosFalloffStart;
    int isBlinnPhong; //BlinnPhongReflectionを行うかどうか
    int enablSpotLighting; //点光源を有効にするか
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
ConstantBuffer<SpotLight> gSpotLight : register(b4);

//ライティング
struct Lighting {
    float3 diffuse;
    float3 specular;
};

//平行光源
Lighting DirectionalLighting(VertexShaderOutput input, float4 textureColor, float3 toEye) {
     //ライティング
    Lighting result;
    
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
    
    //NdotL = step(0.25f, NdotL);
    result.diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NDotDirectional * gDirectionalLight.intensity;
        
    //鏡面反射
    result.specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);

    return result;
}

//点光源
Lighting PointLighing(VertexShaderOutput input, float4 textureColor, float3 toEye) {
    //ライティング
    Lighting result;
    
    //点光源の方向ベクトル
    float3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
   
    //コサイン
    float NDotPoint = saturate(dot(normalize(input.normal), -pointLightDirection));
    
    //鏡面反射の強度
    float specularPow = 0.0f;
    if (gPointLight.isBlinnPhong) {
        //BlingPhongReflectionModel
        //ハーフベクトル
        float3 halfVector = normalize(-pointLightDirection + toEye);
        float NDotH = dot(normalize(input.normal), halfVector);
        //反射強度
        specularPow = pow(saturate(NDotH), gMaterial.shininess);
    } else {
        //PhongReflectionModel
        //入射光の反射ベクトルを求める
        float3 reflectLight = reflect(pointLightDirection, normalize(input.normal));
        //鏡面反射の強度
        float RdotE = dot(reflectLight, toEye);
        //反射強度
        specularPow = pow(saturate(RdotE), gMaterial.shininess);
    }
    
    //距離減衰
    float distance = length(gPointLight.position - input.worldPosition);
    float attenuation = 1.0f / pow(distance, 2.0f);
    
    //拡散反射
    result.diffuse = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * NDotPoint * gPointLight.intensity;
    //鏡面反射
    result.specular = gPointLight.color.rgb * gPointLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
    
    //距離減衰を適応
    result.diffuse *= attenuation;
    result.specular *= attenuation;
    return result;
}

//スポットライト
Lighting SpotLightint(VertexShaderOutput input, float4 textureColor, float3 toEye) {
    //ライティング
    Lighting result;
    
    //スポットライトの入射光
    float3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.position);
    
    //コサイン
    float NDotSpot = saturate(dot(normalize(input.normal), -spotLightDirectionOnSurface));
        
     //鏡面反射の強度
    float specularPow = 0.0f;
    if (gSpotLight.isBlinnPhong) {
        //BlingPhongReflectionModel
        //ハーフベクトル
        float3 halfVector = normalize(-spotLightDirectionOnSurface + toEye);
        float NDotH = dot(normalize(input.normal), halfVector);
        //反射強度
        specularPow = pow(saturate(NDotH), gMaterial.shininess);
    } else {
        //PhongReflectionModel
        //入射光の反射ベクトルを求める
        float3 reflectLight = reflect(spotLightDirectionOnSurface, normalize(input.normal));
        //鏡面反射の強度
        float RdotE = dot(reflectLight, toEye);
        //反射強度
        specularPow = pow(saturate(RdotE), gMaterial.shininess);
    }
        
    //拡散反射
    result.diffuse = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * NDotSpot * gSpotLight.intensity;

    //鏡面反射
    result.specular = gSpotLight.color.rgb * gSpotLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        
    float cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
    float falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));
        
    //光の減衰
    float attenuation = 1.0f / (gSpotLight.distance * gSpotLight.decay);
        
    //光の減衰を適応
    result.diffuse *= attenuation * falloffFactor;
    result.specular *= attenuation * falloffFactor;
    
    return result;
}

//ライティングのモードを切り替える用の関数
float3 SetLightingMode(float3 baseColor, VertexShaderOutput input, float4 textureColor, float3 toEye) {
    //平行光源
    Lighting directionalLighting = DirectionalLighting(input, textureColor, toEye);
        
    //点光源
    Lighting pointLighting = PointLighing(input, textureColor, toEye);
    
    //スポットライト
    Lighting spotLight = SpotLightint(input, textureColor, toEye);
    
    //拡散反射+鏡面反射
    if (gDirectionalLight.enableDirectonalLighting) {
        //平行光源が有効だった場合
        baseColor = directionalLighting.diffuse + directionalLighting.specular;
    } else if (gPointLight.enablePointLighting) {
        //点光源が有効だった場合
        baseColor = pointLighting.diffuse + pointLighting.specular;
    } else if (gSpotLight.enablSpotLighting) {
        baseColor = spotLight.diffuse + spotLight.specular;
    }
    
    //両方が有効だった場合
    if (gDirectionalLight.enableDirectonalLighting && gPointLight.enablePointLighting) {
        baseColor = directionalLighting.diffuse + directionalLighting.specular + pointLighting.diffuse + pointLighting.specular;
    } else if (gDirectionalLight.enableDirectonalLighting && gSpotLight.enablSpotLighting) {
        baseColor = directionalLighting.diffuse + directionalLighting.specular + spotLight.diffuse + spotLight.specular;
    } else if (gPointLight.enablePointLighting && gSpotLight.enablSpotLighting) {
        baseColor = pointLighting.diffuse + pointLighting.specular + spotLight.diffuse + spotLight.specular;
    }
     
    //三つとも有効だった場合
    if (gDirectionalLight.enableDirectonalLighting && gPointLight.enablePointLighting && gSpotLight.enablSpotLighting) {
        baseColor = directionalLighting.diffuse + directionalLighting.specular + pointLighting.diffuse + pointLighting.specular + spotLight.diffuse + spotLight.specular;
    }
    
    return baseColor;
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
        
        //ライティング
        output.color.rgb = SetLightingMode(output.color.rgb, input, textureColor, toEye);
      
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