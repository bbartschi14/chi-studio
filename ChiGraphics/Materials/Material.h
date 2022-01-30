#pragma once
#define GLM_PRECISION_HIGHP_DOUBLE
#define NOMINMAX

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include "ChiGraphics/Utilities.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include "../Keyframing/Keyframeable.h"
#include "../Keyframing/KeyframeTrack.h"
#include "ChiGraphics/Textures/FImage.h"
#include  <iostream>
#include "ChiGraphics/RNG.h"

namespace CHISTUDIO {
    
class Material : public IKeyframeable
{
public:
    Material()
        : IKeyframeable("Material"),
        Albedo(glm::dvec3(1.0, 0., 0.)),
        AlbedoMap(nullptr),
        RoughnessMap(nullptr),
        bRoughnessMapIsSpecular(false),
        MetallicMap(nullptr),
        EmittanceMap(nullptr),
        BumpMap(nullptr),
        AlphaMap(nullptr),
        Roughness(1.0f),
        Metallic(0.0f),
        Emittance(0.0f),
        IndexOfRefraction(1.5f),
        bIsTransparent(false)
    {

    }

    Material(const glm::dvec3& InAlbedo, 
             const float& InRoughness, 
             const float& InMetallic, 
             const float& InEmittance, 
             const float& InIndexOfRefraction, 
             const bool& InIsTransparent)
        : IKeyframeable("Material"), 
        Albedo(InAlbedo),
        AlbedoMap(nullptr),
        RoughnessMap(nullptr),
        bRoughnessMapIsSpecular(false),
        MetallicMap(nullptr),
        EmittanceMap(nullptr),
        BumpMap(nullptr),
        AlphaMap(nullptr),
        Roughness(InRoughness),
        Metallic(InMetallic),
        Emittance(InEmittance),
        IndexOfRefraction(InIndexOfRefraction),
        bIsTransparent(InIsTransparent)
    {
    }

    static const std::shared_ptr<Material> GetDefault() {
        return MakeDiffuse(glm::dvec3(1.0, .1, .05));
    }

    static const std::shared_ptr<Material> MakeDiffuse(glm::dvec3 InAlbedo) {
        return std::make_shared<Material>(
            InAlbedo,   // Albedo
            1.0f,       // Roughness    
            0.0f,       // Metallic
            0.0f,       // Emittance
            1.5f,       // IndexOfRefraction
            false       // IsTransparent
        );
    }

    /*
    * Evaluate the Bidirectional Scattering Distribution Function for this material's properties.
    * 
    * Parameters 'InTowardViewer' and 'InTowardIncident' correspond to w_o and w_i in the rendering equation.
    * The distribution function used is the Beckmann Microfacet Distribution Model (https://www.pbr-book.org/3ed-2018/Reflection_Models/Microfacet_Models).
    * The specular component uses the Cook-Torrance BRDF, while the diffuse component uses Lambert's BRDF.
    *
    * Other references:
    * https://learnopengl.com/PBR/Theory
    * http://www.codinglabs.net/article_physically_based_rendering.aspx
    * http://www.codinglabs.net/article_physically_based_rendering_cook_torrance.aspx
    * https://agraphicsguy.wordpress.com/2015/11/01/sampling-microfacet-brdf/
    */
    glm::dvec3 EvaluateBSDF(glm::dvec3 InSurfaceNormal, glm::dvec3 InTowardViewer, glm::dvec3 InTowardIncident, const glm::vec2& InUVs, RNG& InRNG) const
    {
        double NormalDottedWithViewer = glm::dot(InSurfaceNormal,InTowardViewer);
        double NormalDottedWithIncident = glm::dot(InSurfaceNormal, InTowardIncident);

        bool bIsViewerOutside = NormalDottedWithViewer > 0.0;
        bool bIsIncidentOutside = NormalDottedWithIncident > 0.0;

        float sampledRoughness = SampleRoughness(InUVs);
        glm::dvec3 sampledAlbedo = SampleAlbedo(InUVs);
        float sampledMetallic = SampleMetallic(InUVs);

        // Check if we're looking for transmitted light
        if (!bIsViewerOutside || !bIsIncidentOutside) {
            if (!bIsTransparent) // If the material is opaque, don't transmit any light
                return glm::dvec3(0.0);
        }

        // Check for diffuse/specular reflection
        if (bIsViewerOutside == bIsIncidentOutside)
        {
            // Calculate halfway vector
            glm::dvec3 halfway = glm::normalize(InTowardIncident + InTowardViewer);
            double ViewerDotHalfway = glm::dot(InTowardViewer, halfway);
            double NormalDotHalfway = glm::dot(InSurfaceNormal, halfway);
            
            // Evaluate microfacet distribution function
            // D = exp(((n dot h)^2 - 1) / (roughness^2 (n dot h)^2)) / (pi * roughness^2 (n dot h)^4)
            double NH2 = NormalDotHalfway * NormalDotHalfway;
            double roughnessSquared = sampledRoughness * sampledRoughness;
            double dNum1 = (NH2 - 1.0);
            double dNum2 = (roughnessSquared * NH2);
            double dNumerator = glm::exp(dNum1 / dNum2);
            double dDenominator = (kPi * roughnessSquared * NH2 * NH2);
            double distributionFunction = dNumerator / dDenominator;

            // Evaluate fresnel using schlick's approximation
            // F = F0 + (1 - F0)(1 - wi dot h)^5
            glm::dvec3 fresnel;
            if (!bIsIncidentOutside && glm::sqrt(1.0 - ViewerDotHalfway * ViewerDotHalfway) * IndexOfRefraction > 1.0)
            {
                // Here we have total internal reflection
                fresnel = glm::dvec3(1.0, 1.0, 1.0);
            }
            else 
            {
                double f0 = glm::pow(((IndexOfRefraction - 1.0) / (IndexOfRefraction + 1.0)), 2);
                glm::dvec3 colorF0 = glm::lerp(glm::dvec3(f0, f0, f0), sampledAlbedo, (double)sampledMetallic);
                fresnel = colorF0 + (glm::dvec3(1.0, 1.0, 1.0) - colorF0) * glm::pow((1.0 - ViewerDotHalfway), 5);
            };

            // Evaluate geometry function using microfacet shadowing
            // G = min(1, 2(n dot h)(n dot wo)/(wo dot h), 2(n dot h)(n dot wi)/(wo dot h))
            double geometryFunction = glm::min(NormalDottedWithIncident * NormalDotHalfway, NormalDottedWithViewer * NormalDotHalfway);
            geometryFunction = (2.0 * geometryFunction) / ViewerDotHalfway;
            geometryFunction = glm::min(1.0, geometryFunction);

            // Evaluate BRDF (combine specular and diffuse)
            // Cook-Torrance = DFG / (4(n dot wi)(n dot wo))
            // Lambert = (1 - F) * c / pi
            glm::dvec3 specularComponent = distributionFunction * fresnel * geometryFunction / (4.0 * NormalDottedWithViewer * NormalDottedWithIncident);

            if (bIsTransparent) 
            {
                return specularComponent;
            }
            else 
            {
                glm::dvec3 diffuseComponent = (glm::dvec3(1.0, 1.0, 1.0) - fresnel) * sampledAlbedo / (double)kPi;
                return specularComponent + diffuseComponent;
            }
        }
        else
        {
            // Refraction/Transmission
            // 
            // Ratio of refractive indices
            double etaT = bIsViewerOutside ? IndexOfRefraction : 1.0f / IndexOfRefraction;

            glm::dvec3 preNorm = InTowardIncident * etaT + InTowardViewer;
            glm::dvec3 halfway = glm::normalize(InTowardIncident * etaT + InTowardViewer);
            double ViewerDotHalfway = glm::dot(InTowardViewer, halfway);
            double IncidentDotHalfway = glm::dot(InTowardIncident, halfway);
            double NormalDotHalfway = glm::dot<3, double, glm::qualifier::highp>(InSurfaceNormal, halfway);

            // Evaluate microfacet distribution function
            // D = exp(((n dot h)^2 - 1) / (roughness^2 (n dot h)^2)) / (pi * roughness^2 (n dot h)^4)
            double NH2 = glm::pow<double, int, glm::qualifier::highp>(NormalDotHalfway, 2);
            double roughnessSquared = sampledRoughness * sampledRoughness;
            double top = (NH2 - 1.0);
            double bottom = (roughnessSquared * NH2);
            double preExp = top / bottom;
            double distributionNumerator = glm::exp(preExp);
            double distributionDenominator = (kPi * roughnessSquared * NH2 * NH2);
            double distributionFunction = distributionNumerator / distributionDenominator;

            // Evaluate fresnel using schlick's approximation
            // F = F0 + (1 - F0)(1 - wi dot h)^5
            double f0 = glm::pow(((IndexOfRefraction - 1.0) / (IndexOfRefraction + 1.0)), 2);
            glm::dvec3 colorF0 = glm::lerp(glm::dvec3(f0, f0, f0), sampledAlbedo, (double)sampledMetallic);
            glm::dvec3 fresnel = colorF0 + (glm::dvec3(1.0, 1.0, 1.0) - colorF0) * glm::pow((1.0 - glm::abs(ViewerDotHalfway)), 5);

            // Evaluate geometry function using microfacet shadowing
            // G = min(1, 2(n dot h)(n dot wo)/(wo dot h), 2(n dot h)(n dot wi)/(wo dot h))
            double geometryFunction = glm::min(glm::abs(NormalDottedWithIncident * NormalDotHalfway), glm::abs(NormalDottedWithViewer * NormalDotHalfway));
            geometryFunction = (2.0f * geometryFunction) / glm::abs(ViewerDotHalfway);
            geometryFunction = glm::min(1.0, geometryFunction);

            // Evaluate BTDF (https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf)
            // Cook-Torrance = |h dot wi|/|n dot wi| * |h dot wo|/|n dot wo|
            //                  * n_o^2 (1 - F)DG / (n_i (h dot wi) + n_o (h dot wo))^2
            glm::dvec3 btdf = glm::abs(IncidentDotHalfway * ViewerDotHalfway / (NormalDottedWithIncident * NormalDottedWithViewer))
                * (distributionFunction * (glm::dvec3(1.0, 1.0, 1.0) - fresnel) * geometryFunction / glm::pow((etaT * IncidentDotHalfway + ViewerDotHalfway), 2));

            return btdf * sampledAlbedo;
        }
    }

    /*
    * Sample hemisphere around a surface normal for an out direction and a 
    * corresponding probability density function. Uses the Beckmann distribution
    * function.
    * 
    * Parameter 'InTowardViewer' corresponds to w_o in the rendering equation.
    * PDF : Probability Density Function
    * 
    * Returns false if ray shouldn't be used
    */
    bool SampleHemisphere(glm::dvec3& OutDirection, double& OutPDF, glm::dvec3 InSurfaceNormal, glm::dvec3 InTowardViewer, const glm::vec2& InUVs, RNG& InRNG)
    {
        // https://agraphicsguy.wordpress.com/2015/11/01/sampling-microfacet-brdf/

        float sampledRoughness = SampleRoughness(InUVs);
        glm::dvec3 sampledAlbedo = SampleAlbedo(InUVs);
        float sampledMetallic = SampleMetallic(InUVs);

        double roughnessSquared = (double)sampledRoughness * (double)sampledRoughness;

        // Using the Fresnel term, estimate specular contribution 
        double f0 = glm::pow(((IndexOfRefraction - 1.0) / (IndexOfRefraction + 1.0)), 2);
        double f = (1.0 - sampledMetallic) * f0 + sampledMetallic * ((sampledAlbedo.x + sampledAlbedo.y + sampledAlbedo.z) / 3.0);
        f = glm::lerp(f, 1.0, 0.2);

        double etaT = glm::dot(InTowardViewer, InSurfaceNormal) > 0.0 ? IndexOfRefraction : 1.0 / IndexOfRefraction;

        auto beckmannHalfwayVector = [&]()
        {
            // Probability integral transform for Beckmann distribution microfacet normal
            // theta = arctan sqrt(-m^2 ln U)
            double theta = glm::atan(glm::sqrt(-roughnessSquared * glm::log(InRNG.Float())));
            double sinT = glm::sin(theta);
            double cosT = glm::cos(theta);

            // Generate halfway vector by sampling azimuth uniformly
            glm::dvec2 point = RandomInUnitDisk(InRNG);
            glm::dvec3 halfway = glm::dvec3(point.x * sinT, point.y * sinT, cosT);
            
            //std::cout << glm::to_string(halfway) << std::endl;
            return GetLocalToWorld(InSurfaceNormal) * halfway;
        };

        auto beckmannPDF = [&](glm::dvec3 InHalfway, glm::dvec3 InNormal)
        {
            // p = 1 / (pi m^2 cos^3 theta) * e^(-tan^2(theta) / m^2)
            double cosT = glm::min(glm::abs(glm::dot(InHalfway, InNormal)), 1.0);
            double sinT = glm::sqrt(1.0 - cosT * cosT);
            double denom = 1.0 / (kPi * roughnessSquared * glm::pow(cosT, 3));
            double secondTerm = glm::exp(-glm::pow(sinT / cosT, 2) / roughnessSquared);
            double probability = denom * secondTerm;
            
            return probability;
        };
      
        bool bIsReflected = InRNG.Float() <= f;
        glm::dvec3 toIncidentRay; 

        if (bIsReflected)
        {
            // Specular component
            glm::dvec3 halfway = beckmannHalfwayVector();
            toIncidentRay = -glm::reflect(InTowardViewer, halfway);
        }
        else if (!bIsTransparent)
        {
            // Diffuse component (Lambertian)
            // Simple cosine-sampling using Malley's method
            glm::dvec2 point = RandomInUnitDisk(InRNG);
            double z = glm::sqrt((1.0 - point.x * point.x - point.y * point.y));
            toIncidentRay = GetLocalToWorld(InSurfaceNormal) * glm::dvec3(point.x, point.y, z);
        }
        else 
        {
            // Transmitted component
            glm::vec3 truncatedViewer = InTowardViewer;
            glm::vec3 halfway = beckmannHalfwayVector();
            float cosT_Viewer = glm::dot(halfway, truncatedViewer);
            glm::vec3 viewerPerp = truncatedViewer - halfway * cosT_Viewer;
            glm::vec3 incidentPerp = -viewerPerp / (float)etaT;
            float sin2_ti = glm::length2(incidentPerp);
            if (sin2_ti > 1.0)
            {
                // This angle doesn't yield any transmittence to wo,
                // due to total internal reflection
                return false;
            }
            float cos_ti = glm::sqrt(1.0f - sin2_ti);
            toIncidentRay = -glm::sign(cosT_Viewer) * cos_ti * halfway + incidentPerp;
        };

        // Multiple importance sampling  uses the probabilities of several components. Now we sum them up.
        double probability = 0.0;
        {
            // Specular component
            glm::dvec3 halfway = glm::normalize(toIncidentRay + InTowardViewer);
            double probHalfway = beckmannPDF(halfway, InSurfaceNormal);
            double specularComponent = f * probHalfway / (4.0 * glm::abs(glm::dot(halfway, InTowardViewer)));
            probability += specularComponent;
        }
        
        if (!bIsTransparent)
        {
            // Diffuse component
            double dotTerm = glm::dot(toIncidentRay, InSurfaceNormal);
            double clampedDot = glm::max(dotTerm, 0.0);
            double diffuseComponent = (1.0 - f) * dotTerm / kPi;
            probability += diffuseComponent;
        }
        else if (glm::dot(InTowardViewer, InSurfaceNormal) >= 0.0 != glm::dot(toIncidentRay, InSurfaceNormal) >= 0.0) {
            // Transmitted component
            glm::dvec3 halfway = glm::normalize(toIncidentRay * etaT + InTowardViewer);
            double probHalfway = beckmannPDF(halfway, InSurfaceNormal);
            double HDotViewer = glm::dot(halfway, InTowardViewer);
            double HDotIncident = glm::dot(halfway, toIncidentRay);
            double jacobian = glm::abs(HDotViewer) / glm::pow((etaT * HDotIncident + HDotViewer), 2);
            probability += (1.0 - f) * probHalfway * jacobian;
        }
        
        OutDirection = toIncidentRay;
        OutPDF = probability;

        return true;
    }

    glm::dmat3 GetLocalToWorld(glm::dvec3 InNormal)
    {
        glm::dvec3 ns = !std::isnan(InNormal.x) ? glm::normalize(glm::vec3(InNormal.y, -InNormal.x, 0.0)) : glm::normalize(glm::vec3(0.0, -InNormal.z, InNormal.y));
        if (std::isnan(ns.x)) ns = glm::normalize(glm::vec3(0.0, -InNormal.z, InNormal.y));

        glm::dvec3 nss = glm::cross(InNormal, ns);
        return (glm::dmat3(ns.x, ns.y, ns.z, nss.x, nss.y, nss.z, InNormal.x, InNormal.y, InNormal.z));
    }

    // Begin Keyframeable
    void ApplyKeyframeData(int InFrame) override
    {
        if (AlbedoRKeyframeTrack.HasKeyframes())
            Albedo.r = (AlbedoRKeyframeTrack.GetValueAtFrame(InFrame));

        if (AlbedoGKeyframeTrack.HasKeyframes())
            Albedo.g = (AlbedoGKeyframeTrack.GetValueAtFrame(InFrame));

        if (AlbedoBKeyframeTrack.HasKeyframes())
            Albedo.b = (AlbedoBKeyframeTrack.GetValueAtFrame(InFrame));

        if (RoughnessKeyframeTrack.HasKeyframes())        
            Roughness = (RoughnessKeyframeTrack.GetValueAtFrame(InFrame));

        if (MetallicKeyframeTrack.HasKeyframes())
            Metallic = (MetallicKeyframeTrack.GetValueAtFrame(InFrame));
    
        if (EmittanceKeyframeTrack.HasKeyframes())
            Emittance = (EmittanceKeyframeTrack.GetValueAtFrame(InFrame));

        if (IORKeyframeTrack.HasKeyframes())
            IndexOfRefraction = (IORKeyframeTrack.GetValueAtFrame(InFrame));
    }
    std::vector<std::string> GetKeyframeTrackNames() const override
    {
        std::vector<std::string> names = { "Albedo R", "Albedo G", "Albedo B", "Roughness", "Metallic", "Emittance", "IOR" };
        return names;
    }
    void CreateKeyframeOnTrack(std::string InTrackName, int InFrame) override
    {
        if (InTrackName == "Albedo R")
        {
            AlbedoRKeyframeTrack.AddKeyframe(InFrame, Albedo.r);
        }
        else if (InTrackName == "Albedo G")
        {
            AlbedoGKeyframeTrack.AddKeyframe(InFrame, Albedo.g);
        }
        else if (InTrackName == "Albedo B")
        {
            AlbedoBKeyframeTrack.AddKeyframe(InFrame, Albedo.b);
        }
        else if (InTrackName == "Roughness")
        {
            RoughnessKeyframeTrack.AddKeyframe(InFrame, Roughness);
        }
        else if (InTrackName == "Metallic")
        {
            MetallicKeyframeTrack.AddKeyframe(InFrame, Metallic);
        }
        else if (InTrackName == "Emittance")
        {
            EmittanceKeyframeTrack.AddKeyframe(InFrame, Emittance);
        }
        else if (InTrackName == "Albedo B")
        {
            IORKeyframeTrack.AddKeyframe(InFrame, IndexOfRefraction);
        }
    }
    void DeleteKeyframeOnTrack(std::string InTrackName, int InIndex) override
    {
        if (InTrackName == "Albedo R")
        {
            AlbedoRKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
        }
        else if (InTrackName == "Albedo G")
        {
            AlbedoGKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
        }
        else if (InTrackName == "Albedo B")
        {
            AlbedoBKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
        }
        else if (InTrackName == "Roughness")
        {
            RoughnessKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
        }
        else if (InTrackName == "Metallic")
        {
            MetallicKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
        }
        else if (InTrackName == "Emittance")
        {
            EmittanceKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
        }
        else if (InTrackName == "Albedo B")
        {
            IORKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
        }
    }
    std::vector<IKeyframeBase*> GetKeyframesOnTrack(std::string InTrackName) override
    {
        if (InTrackName == "Albedo R")
        {
            return AlbedoRKeyframeTrack.GetKeyframes();
        }
        else if (InTrackName == "Albedo G")
        {
            return AlbedoGKeyframeTrack.GetKeyframes();
        }
        else if (InTrackName == "Albedo B")
        {
            return AlbedoBKeyframeTrack.GetKeyframes();
        }
        else if (InTrackName == "Roughness")
        {
            return RoughnessKeyframeTrack.GetKeyframes();
        }
        else if (InTrackName == "Metallic")
        {
            return MetallicKeyframeTrack.GetKeyframes();
        }
        else if (InTrackName == "Emittance")
        {
            return EmittanceKeyframeTrack.GetKeyframes();
        }
        else if (InTrackName == "Albedo B")
        {
            return IORKeyframeTrack.GetKeyframes();
        }
        return std::vector<IKeyframeBase*>();
    }
    // ~ End Keyframeable

    FKeyframeTrack<float> AlbedoRKeyframeTrack;
    FKeyframeTrack<float> AlbedoGKeyframeTrack;
    FKeyframeTrack<float> AlbedoBKeyframeTrack;
    FKeyframeTrack<float> RoughnessKeyframeTrack;
    FKeyframeTrack<float> MetallicKeyframeTrack;
    FKeyframeTrack<float> EmittanceKeyframeTrack;
    FKeyframeTrack<float> IORKeyframeTrack;

    glm::dvec3 GetAlbedo() const
    {
        return Albedo;
    }

    glm::dvec3 SampleAlbedo(const glm::vec2& InUVs) const
    {
        //return glm::dvec3(InUVs.x, InUVs.y, 0.0f); // Debug UVs
        return AlbedoMap ? AlbedoMap->SampleWithUV(InUVs) : Albedo ;
    }

    void SetAlbedo(const glm::dvec3& InAlbedo)
    {
        Albedo = InAlbedo;
    }

    FImage* GetAlbedoMap() const
    {
        return AlbedoMap;
    }

    void SetAlbedoMap(FImage* InAlbedoMap)
    {
        AlbedoMap = InAlbedoMap;
    }

    float GetRoughness() const
    {
        return Roughness;
    }

    float SampleRoughness(const glm::vec2& InUVs) const
    {
        return RoughnessMap ? (bRoughnessMapIsSpecular ? glm::max(1.0f - RoughnessMap->SampleWithUV(InUVs).x, 0.01f) : RoughnessMap->SampleWithUV(InUVs).x) : Roughness;
    }

    void SetRoughness(const float& InRoughness)
    {
        Roughness = InRoughness;
    }

    float GetMetallic() const
    {
        return Metallic;
    }

    float SampleMetallic(const glm::vec2& InUVs) const
    {
        return MetallicMap ? MetallicMap->SampleWithUV(InUVs).x : Metallic;
    }

    void SetMetallic(const float& InMetallic)
    {
        Metallic = InMetallic;
    }

    float GetEmittance() const
    {
        return Emittance;
    }

    float SampleEmittance(const glm::vec2& InUVs) const
    {
        return EmittanceMap ? EmittanceMap->SampleWithUV(InUVs).x : Emittance;
    }

    void SetEmittance(const float& InEmittance)
    {
        Emittance = InEmittance;
    }

    float GetIndexOfRefraction() const
    {
        return IndexOfRefraction;
    }

    void SetIndexOfRefraction(const float& InIndexOfRefraction)
    {
        IndexOfRefraction = InIndexOfRefraction;
    }

    bool IsTransparent() const
    {
        return bIsTransparent;
    }

    void SetTransparent(const bool& InIsTransparent)
    {
        bIsTransparent = InIsTransparent;
    }

    FImage* GetRoughnessMap() const
    {
        return RoughnessMap;
    }

    void SetRoughnessMap(FImage* InRoughnessMap, bool bIsSpecular)
    {
        bRoughnessMapIsSpecular = bIsSpecular;
        RoughnessMap = InRoughnessMap;
    }

    FImage* GetMetallicMap() const
    {
        return MetallicMap;
    }

    void SetMetallicMap(FImage* InMetallicMap)
    {
        MetallicMap = InMetallicMap;
    }

    FImage* GetEmittanceMap() const
    {
        return EmittanceMap;
    }

    void SetEmittanceMap(FImage* InEmittanceMap)
    {
        EmittanceMap = InEmittanceMap;
    }

    FImage* GetBumpMap() const
    {
        return BumpMap;
    }

    float SampleBump(const glm::vec2& InUVs) const
    {
        return BumpMap ? BumpMap->SampleWithUV(InUVs).x : 0.0f;
    }

    void SetBumpMap(FImage* InBumpMap)
    {
        BumpMap = InBumpMap;
    }

    FImage* GetAlphaMap() const
    {
        return AlphaMap;
    }

    float SampleAlpha(const glm::vec2& InUVs) const
    {
        return AlphaMap ? AlphaMap->SampleWithUV(InUVs).x : 1.0f;
    }

    void SetAlphaMap(FImage* InAlphaMap)
    {
        AlphaMap = InAlphaMap;
    }
private:
    // Base color
    glm::dvec3 Albedo;
    FImage* AlbedoMap;

    // Used for calculating shininess/microfacet distribution
    float Roughness;
    FImage* RoughnessMap;
    bool bRoughnessMapIsSpecular;

    // Metallic vs. dialectic (an example dialectric being plastic)
    float Metallic;
    FImage* MetallicMap;

    // Self emission of light
    float Emittance;
    FImage* EmittanceMap;

    // Used for refraction, snell's law
    float IndexOfRefraction;

    // Used for allowing transmittance
    bool bIsTransparent;

    FImage* BumpMap;
    FImage* AlphaMap;

};

}