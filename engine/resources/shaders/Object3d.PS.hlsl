#include "Object3d.hlsli"

//マテリアル
struct Material {
    float32_t4 color;
    int32_t enableLighring;
    float32_t4x4 uvMatrix;
    float32_t shininess;
};

//平行光源
struct DirectionalLight {
    float32_t4 color; //ライトの色
    float32_t3 direction; //ライトの向き
    float32_t intensity; //輝度
    int32_t isLambert; //lambertにするかどうか
    int32_t isBlinnPhong; //BlinnPhongReflectionを行うかどうか
    int32_t enableDirectonalLighting; //平行光源を有効にするか
};

//点光源
struct PointLight {
    float32_t4 color; //ライトの色
    float32_t3 position; //ライトの位置
    float32_t intensity; //輝度
    float32_t distance; //ライトが届く最大距離
    float32_t decay; //減衰率
    int32_t isBlinnPhong; //BlinnPhongReflectionを行うかどうか
    int32_t enablePointLighting; //点光源を有効にするか
};

//スポットライト
struct SpotLight {
    float32_t4 color; //ライトの色
    float32_t3 position; //ライトの位置
    float32_t intensity; //輝度
    float32_t3 direction; //スポットライトの方向
    float32_t distance; //ライトの届く最大距離
    float32_t decay; //減衰率
    float32_t cosAngle; //スポットライトの余弦
    float32_t cosFalloffStart;
    int32_t isBlinnPhong; //BlinnPhongReflectionを行うかどうか
    int32_t enablSpotLighting; //点光源を有効にするか
};

//カメラ
struct Camera {
    float32_t3 worldPosition;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(t1);
ConstantBuffer<SpotLight> gSpotLight : register(b4);

//ライティング
struct Lighting {
    float32_t3 diffuse;
    float32_t3 specular;
};

//平行光源
Lighting DirectionalLighting(VertexShaderOutput input, float32_t4 textureColor, float32_t3 toEye) {
     //ライティング
    Lighting result;
    
    //コサイン
    float32_t NDotDirectional = 0.0f;
        
    //ライティングの方法
    if (gDirectionalLight.isLambert) {
        //lambert
        NDotDirectional = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
    } else {
        //half lambert
        float32_t NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        //拡散反射
        NDotDirectional = pow(NdotL * 0.5f + 0.5f, 2.0f);
    }
        
    //鏡面反射の強度
    float32_t specularPow = 0.0f;
    if (gDirectionalLight.isBlinnPhong) {
        //BlingPhongReflectionModel
        //ハーフベクトル
        float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        float32_t NDotH = dot(normalize(input.normal), halfVector);
        specularPow = pow(saturate(NDotH), gMaterial.shininess); //反射強度
    } else {
        //PhongReflectionModel
        //入射光の反射ベクトルを求める
        float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
        //鏡面反射の強度
        float32_t RdotE = dot(reflectLight, toEye);
        specularPow = pow(saturate(RdotE), gMaterial.shininess); //反射強度
    }
    
    //NdotL = step(0.25f, NdotL);
    result.diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NDotDirectional * gDirectionalLight.intensity;
        
    //鏡面反射
    result.specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);

    return result;
}

//点光源
Lighting PointLighing(VertexShaderOutput input, float32_t4 textureColor, float32_t3 toEye) {
    //ライティング
    Lighting result;
    
    //点光源の方向ベクトル
    float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
   
    //コサイン
    float32_t NDotPoint = saturate(dot(normalize(input.normal), -pointLightDirection));
    
    //鏡面反射の強度
    float32_t specularPow = 0.0f;
    if (gPointLight.isBlinnPhong) {
        //BlingPhongReflectionModel
        //ハーフベクトル
        float32_t3 halfVector = normalize(-pointLightDirection + toEye);
        float32_t NDotH = dot(normalize(input.normal), halfVector);
        //反射強度
        specularPow = pow(saturate(NDotH), gMaterial.shininess);
    } else {
        //PhongReflectionModel
        //入射光の反射ベクトルを求める
        float32_t3 reflectLight = reflect(pointLightDirection, normalize(input.normal));
        //鏡面反射の強度
        float32_t RdotE = dot(reflectLight, toEye);
        //反射強度
        specularPow = pow(saturate(RdotE), gMaterial.shininess);
    }
    
    //距離減衰
    float32_t attenuation = 1.0f / (gPointLight.distance * gPointLight.decay);
    
    //拡散反射
    result.diffuse = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * NDotPoint * gPointLight.intensity;
    //鏡面反射
    result.specular = gPointLight.color.rgb * gPointLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
    
    //距離減衰を適応
    result.diffuse *= attenuation;
    result.specular *= attenuation;
    return result;
}

//スポットライト
Lighting SpotLightint(VertexShaderOutput input, float32_t4 textureColor, float32_t3 toEye) {
    //ライティング
    Lighting result;
    
    //スポットライトの入射光
    float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.position);
    
    //コサイン
    float32_t NDotSpot = saturate(dot(normalize(input.normal), -spotLightDirectionOnSurface));
        
     //鏡面反射の強度
    float specularPow = 0.0f;
    if (gSpotLight.isBlinnPhong) {
        //BlingPhongReflectionModel
        //ハーフベクトル
        float32_t3 halfVector = normalize(-spotLightDirectionOnSurface + toEye);
        float32_t NDotH = dot(normalize(input.normal), halfVector);
        //反射強度
        specularPow = pow(saturate(NDotH), gMaterial.shininess);
    } else {
        //PhongReflectionModel
        //入射光の反射ベクトルを求める
        float32_t3 reflectLight = reflect(spotLightDirectionOnSurface, normalize(input.normal));
        //鏡面反射の強度
        float32_t RdotE = dot(reflectLight, toEye);
        //反射強度
        specularPow = pow(saturate(RdotE), gMaterial.shininess);
    }
        
    //拡散反射
    result.diffuse = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * NDotSpot * gSpotLight.intensity;

    //鏡面反射
    result.specular = gSpotLight.color.rgb * gSpotLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        
    float32_t cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
    float32_t falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));
        
    //光の減衰
    float32_t attenuation = 1.0f / (gSpotLight.distance * gSpotLight.decay);
        
    //光の減衰を適応
    result.diffuse *= attenuation * falloffFactor;
    result.specular *= attenuation * falloffFactor;
    
    return result;
}

//ライティングのモードを切り替える用の関数
float32_t3 SetLightingMode(VertexShaderOutput input, float32_t4 textureColor, float32_t3 toEye) {
   //ライティング
        //平行光源
    Lighting directionalLighting = DirectionalLighting(input, textureColor, toEye);
        
        //点光源
    Lighting pointLighting = PointLighing(input, textureColor, toEye);
    
        //スポットライト
    Lighting spotLight = SpotLightint(input, textureColor, toEye);
        
    float32_t3 directionalRgb = { 0.0f, 0.0f, 0.0f };
    if (gDirectionalLight.enableDirectonalLighting) {
        directionalRgb = directionalLighting.diffuse + directionalLighting.specular;
    }
        
    float32_t3 pointRgb = { 0.0f, 0.0f, 0.0f };
    if (gPointLight.enablePointLighting) {
        pointRgb = pointLighting.diffuse + pointLighting.specular;
    }
        
    float32_t3 spotRgb = { 0.0f, 0.0f, 0.0f };
    if (gSpotLight.enablSpotLighting) {
        spotRgb = spotLight.diffuse + spotLight.specular;
    }
    
    return directionalRgb + pointRgb + spotRgb;
}

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvMatrix);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    PixelShaderOutput output;
    
    //Lightingする場合
    if (gMaterial.enableLighring) {
        //カメラの視点を取得
        float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        
        
        //ライティングをRGBに適応
        output.color.rgb = SetLightingMode(input, textureColor, toEye);
      
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