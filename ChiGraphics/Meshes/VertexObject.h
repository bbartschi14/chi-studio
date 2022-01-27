#pragma once

#include "ChiGraphics/GL_Wrapper/VertexArray.h"
#include "Primitives/FFace.h"
#include "Primitives/FVertex.h"
#include "Primitives/FHalfEdge.h"
#include "Primitives/FEdge.h"
#include <set>
#include "core.h"
#include <unordered_map>

namespace CHISTUDIO {

    enum class EFaceExtrudeType
    {
        Individual,
        Regions
    };

    enum class EDefaultObject
    {
        Cube,
        CustomMesh,
        Debug,
        Plane,
        Cylinder,
    };

    struct FDefaultObjectParams
    {
        FDefaultObjectParams() : NumberOfSides(0) {};
        int NumberOfSides;
    };

    enum class EShadingType
    {
        Flat = 0,
        Smooth = 1
    };

    struct FIncidentHalfEdges
    {
        std::vector<FHalfEdge*> IncidentHalfEdges;
    };

    struct FBoundaryLoop {
        std::vector<FHalfEdge*> Loop;
        std::vector<FIncidentHalfEdges> IncidentHalfEdgesPerLoopEdge;

        std::string ToString()
        {
            std::string result = "Boundary Loop: [";
            for (FHalfEdge* halfEdge : Loop)
            {
                result += fmt::format("{}, ", halfEdge->GetIndexId());
            }
            result += "]\n";
            return result;
        }
    };

    struct FFaceRegion
    {
        std::vector<FFace*> FacesInRegion;
        std::vector<std::shared_ptr<FBoundaryLoop>> BoundaryLoops;
        std::vector<FHalfEdge*> UnsortedLoopEdges;

        glm::vec3 GetAverageNaiveNormal()
        {
            glm::vec3 result = glm::vec3(0.0f);
            for (FFace* face : FacesInRegion)
            {
                result += face->CalculateNaiveFaceNormal();
            }
            result /= FacesInRegion.size();
            return glm::normalize(result);
        }

        std::string ToString()
        {
            std::string result = "Faces: [";
            for (FFace* face : FacesInRegion)
            {
                result += fmt::format("{}, ", face->GetIndexId());
            }
            result += "]\n";

            for (size_t i = 0; i < BoundaryLoops.size(); i++)
            {
                result += BoundaryLoops[i]->ToString();
            }

            return result;
        }
    };

    struct FHalfEdgeMeshData
    {
        std::vector<std::unique_ptr<FFace>> Faces;
        std::vector<std::unique_ptr<FHalfEdge>> HalfEdges;
        std::vector<std::unique_ptr<FEdge>> Edges;
        std::vector<std::unique_ptr<FVertex>> Vertices;

        int FaceIndex;
        int HalfEdgeIndex;
        int EdgeIndex;
        int VertexIndex;
    };

    // Instances of this class store various vertex data and are responsible
    // for sending data from CPU to GPU via the Update* methods.
    class VertexObject {
    public:
        VertexObject(EDefaultObject InDefaultObject, FDefaultObjectParams InParams) : VertexArray_(make_unique<VertexArray>()),
            EdgeVertexArray_(make_unique<VertexArray>()),
            SelectedFaceVertexArray_(make_unique<VertexArray>()),
            SelectedEdgeVertexArray_(make_unique<VertexArray>()),
            SelectedVertexVertexArray_(make_unique<VertexArray>()),
            bDebugNormals(false),
            NormalsDebugVertexArray_(make_unique<VertexArray>()),
            bUseImportedNormals(false)
        {
            ShadingType = EShadingType::Flat;

            SelectedVertexVertexArray_->CreatePositionBuffer();
            SelectedVertexVertexArray_->SetDrawMode(EDrawMode::Points);

            NormalsDebugVertexArray_->CreatePositionBuffer();
            NormalsDebugVertexArray_->SetDrawMode(EDrawMode::Lines);

            EdgeVertexArray_->CreatePositionBuffer();
            EdgeVertexArray_->SetDrawMode(EDrawMode::Lines);

            SelectedEdgeVertexArray_->CreatePositionBuffer();
            SelectedEdgeVertexArray_->SetDrawMode(EDrawMode::Lines);

            SelectedFaceVertexArray_->CreatePositionBuffer();
            SelectedFaceVertexArray_->CreateIndexBuffer();

            if (InDefaultObject == EDefaultObject::Cube)
            {
                CreateHalfEdgeCube();
            }
            else if (InDefaultObject == EDefaultObject::Plane)
            {
                CreateHalfEdgePlane();
            }
            else if (InDefaultObject == EDefaultObject::Cylinder)
            {
                CreateHalfEdgeCylinder(InParams);
            }


            if (InDefaultObject != EDefaultObject::Debug && InDefaultObject != EDefaultObject::CustomMesh)
            {
                CreateVertexArrayFromHalfEdgeStructure();
            }
        }

        // Vertex buffers are created in a lazy manner in the following Update*.
        void UpdatePositions(std::unique_ptr<FPositionArray> InPositions);
        void UpdateNormals(std::unique_ptr<FNormalArray> InNormals);
        void UpdateColors(std::unique_ptr<FColorArray> InColors);
        void UpdateTexCoord(std::unique_ptr<FTexCoordArray> InTexCoords);
        void UpdateIndices(std::unique_ptr<FIndexArray> InIndices);

        // Create a deep copy of another vertex object
        void CopyVertexObject(VertexObject* InVertexObject);

        /** Using the given faces (InFaces) on this vertex object, find all the contiguous regions */
        std::vector<std::unique_ptr<FFaceRegion>> FindRegionsFromFaces(std::vector<FFace*> InFaces) const;
        void SortRegionBoundaryLoops(FFaceRegion* InFaceRegion) const;

        void SelectEdgeLoop(FEdge* InStartingEdge);

        // Create faces bridging between the two boundary loops. Both boundary loops should be the same size,
        // and each half edge on the loop 
        void BridgeBoundaryLoops(FBoundaryLoop* loopOne, FBoundaryLoop* loopTwo);

        void DeleteFaces(std::vector<FFace*> InFaces);
    private:

        void CreateHalfEdgeCube();
        void CreateHalfEdgePlane();
        void CreateHalfEdgeCylinder(FDefaultObjectParams InParams);
        void CreateVertexArrayFromHalfEdgeStructure();

        /** Checks for nullptrs across all half edge primitives */
        void VerifyHalfEdgeStability();
    public:
        bool HasPositions() const {
            return Positions != nullptr;
        }

        bool HasNormals() const {
            return Normals != nullptr;
        }

        bool HasColors() const {
            return Colors != nullptr;
        }

        bool HasTexCoords() const {
            return TexCoords != nullptr;
        }

        bool HasIndices() const {
            return Indices != nullptr;
        }

        const FPositionArray& GetPositions() const {
            if (Positions == nullptr)
                throw std::runtime_error("No position in VertexObject!");
            return *Positions;
        }

        const FNormalArray& GetNormals() const {
            if (Normals == nullptr)
                throw std::runtime_error("No normal in VertexObject!");
            return *Normals;
        }

        const FColorArray& GetColors() const {
            if (Colors == nullptr)
                throw std::runtime_error("No color in VertexObject!");
            return *Colors;
        }

        const FTexCoordArray& GetTexCoords() const {
            if (TexCoords == nullptr)
                throw std::runtime_error("No texture coordinate in VertexObject!");
            return *TexCoords;
        }

        const FIndexArray& GetIndices() const {
            if (Indices == nullptr)
                throw std::runtime_error("No indices in VertexObject!");
            return *Indices;
        }

        VertexArray& GetVertexArray() {
            return *VertexArray_.get();
        }
        const VertexArray& GetVertexArray() const {
            return *VertexArray_.get();
        }

        VertexArray& GetEdgeVertexArray() {
            return *EdgeVertexArray_.get();
        }

        const std::vector<std::unique_ptr<FFace>>& GetFaces() const
        {
            return Faces;
        }

        const std::vector<std::unique_ptr<FHalfEdge>>& GetHalfEdges() const
        {
            return HalfEdges;
        }

        const std::vector<std::unique_ptr<FEdge>>& GetEdges() const
        {
            return Edges;
        }

        const std::vector<std::unique_ptr<FVertex>>& GetVertices() const
        {
            return Vertices;
        }

        int GetFaceIndex() const
        {
            return FaceIndex;
        }
        int GetHalfEdgeIndex() const
        {
            return HalfEdgeIndex;
        }
        int GetEdgeIndex() const
        {
            return EdgeIndex;
        }
        int GetVertexIndex() const
        {
            return VertexIndex;
        }

        const std::set<int>& GetSelectedFaces() const
        {
            return SelectedFaces;
        }

        std::vector<FFace*> GetSelectedFacesPtrs() const
        {
            std::vector<FFace*> faces;
            for (size_t i = 0; i < Faces.size(); i++)
            {
                if (!(SelectedFaces.find(Faces[i]->GetIndexId()) == SelectedFaces.end()))
                {
                    faces.push_back(Faces[i].get());
                }
            }
            return faces;
        }


        const std::set<int>& GetSelectedHalfEdges() const
        {
            return SelectedHalfEdges;
        }

        const std::set<int>& GetSelectedEdges() const
        {
            return SelectedEdges;
        }

        std::vector<FEdge*> GetSelectedEdgesPtrs() const
        {
            std::vector<FEdge*> edges;
            for (size_t i = 0; i < Edges.size(); i++)
            {
                if (!(SelectedEdges.find(Edges[i]->GetIndexId()) == SelectedEdges.end()))
                {
                    edges.push_back(Edges[i].get());
                }
            }
            return edges;
        }

        const std::set<int>& GetSelectedVertices() const
        {
            return SelectedVertices;
        }

        std::vector<FVertex*> GetSelectedVerticesPtrs() const
        {
            std::vector<FVertex*> vertices;
            for (size_t i = 0; i < Vertices.size(); i++)
            {
                if (!(SelectedVertices.find(Vertices[i]->GetIndexId()) == SelectedVertices.end()))
                {
                    vertices.push_back(Vertices[i].get());
                }
            }
            return vertices;
        }

        VertexArray& GetSelectedVertexVertexArray() {
            return *SelectedVertexVertexArray_.get();
        }

        VertexArray& GetSelectedEdgeVertexArray() {
            return *SelectedEdgeVertexArray_.get();
        }

        VertexArray& GetSelectedFaceVertexArray() {
            return *SelectedFaceVertexArray_.get();
        }

        void SelectVertex(int InIndex, bool addToSelection);
        void SelectEdge(int InIndex, bool addToSelection);
        void SelectFace(int InIndex, bool addToSelection);

        void SelectAllVertices();
        void SelectAllEdges();
        void SelectAllFaces();

        void DeselectVertex(int InIndex);
        void DeselectEdge(int InIndex);
        void DeselectFace(int InIndex);

        void ClearSelectedVertices();
        void ClearSelectedEdges();
        void ClearSelectedFaces();

        void OnSelectionChanged();

        /** Takes all selected verts, edges, and faces, and returns all the verts that belong to any of them */
        std::set<FVertex*> GetAggregateSelectedVertices() const;
        glm::vec3 GetSelectedPrimAveragePosition();

        size_t GetNumberOfPrimsSelected() const {
            return (SelectedVertices.size() + SelectedEdges.size() + SelectedFaces.size());
        }

        // Deletes all primitives associated with the selected vertices
        void DeleteSelectedVertices();

        // Modifies Vertices array. Do not use while iterating over the former
        void DeleteVertex(FVertex* InVertex);
        // Modifies Edges array and deletes half edges. Do not use while iterating over the former
        void DeleteEdge(FEdge* InEdge);
        // Modifies HalfEdges array. Do not use while iterating over the former. Do not use as starting point, use DeleteEdge
        void DeleteHalfEdge(FHalfEdge* InHalfEdge);
        // Modifies Faces array. Do not use while iterating over the former
        void DeleteFace(FFace* InFace);

        // Remove all primitives marked for deletion
        void CleanupDeletedPrimitives();

        void MergeVerticesByDistance(float InDistance);

        void MoveSelectedPrims(glm::vec3 InDistance);

        // Rotates around average selection position and local object axes
        void RotateSelectedPrims(glm::vec3 InRotation);

        // Scales around average selection position and local object axes
        void ScaleSelectedPrims(glm::vec3 InScale, glm::vec3 InStartingScaleOrigin, std::vector<glm::vec3> InPreScaleVertexPositions);

        void Render();

        bool IsDebugNormals() const {
            return bDebugNormals;
        }

        void SetDebugNormals(bool InDebugNormals);

        VertexArray& GetDebugNormalsVertexArray() {
            return *NormalsDebugVertexArray_.get();
        }

        void MarkDirty()
        {
            CreateVertexArrayFromHalfEdgeStructure();
        }

        void ExtrudeSelectedFaces(EFaceExtrudeType InType);
        void ExtrudeSelectedEdges(EFaceExtrudeType InType, glm::vec3 InExtrudeDelta);
        void ExtrudeEdge(FEdge* InEdgeToExtrude);

        FVertex* CreateVertex(glm::vec3 InPosition, FHalfEdge* InOwningHalfEdge);
        FHalfEdge* CreateHalfEdge(FHalfEdge* InNextHalfEdge, FHalfEdge* InSymmetricalHalfEdge, FFace* InOwningFace, FEdge* InOwningEdge, FVertex* InNextVertex);
        FEdge* CreateEdge(FHalfEdge* InFirstHalfEdge, FHalfEdge* InSecondHalfEdge);
        FFace* CreateFace(FHalfEdge* InHalfEdgeOnFace);

        bool HandleClick(class FRay InSceneRay, class Application* InOriginatingApplication);
        FVertex* GetVertexUnderRay(class FRay InSceneRay);
        FEdge* GetEdgeUnderRay(class FRay InSceneRay);
        FFace* GetFaceUnderRay(class FRay InSceneRay);

        void SetShadingType(EShadingType InShadingType);
        EShadingType GetShadingType() const;

        void SetImportedNormals();
#pragma region Subdivision Surface Functions (Catmull-Clark Subdivision)

        // Calculate and apply a single iteration of subdivision surface
        void ApplySubdivisionSurface();

        // Maps face indices to face centroid positions
        std::unordered_map<int, glm::vec3> GetSubdivisionFaceCentroids() const;

        // Maps edge indices to smoothed edge midpoint positions
        std::unordered_map<int, glm::vec3> GetSmoothedEdgeMidpoints(std::unordered_map<int, glm::vec3> InFaceCentroids) const;

        // Use the edge midpoints and face centroids to update the positions of the original vertices
        void SmoothOriginalVertices(std::unordered_map<int, glm::vec3> InFaceCentroids, std::unordered_map<int, glm::vec3> InEdgeMidpoints);

        // Split a face into 4, using the calculated face centroids and smoothed edge midpoints
        void QuadrangleFace(FFace* InFace, std::unordered_map<int, FVertex*> InFaceCentroidVertices, std::unordered_map<int, FVertex*> InEdgeMidpointVertices, std::set<int>& InAlreadyQuadrangledFaces);

#pragma endregion

    private:
        void ExtrudeRegion(FFaceRegion* InRegion);

        std::vector<std::unique_ptr<FFace>> Faces;
        std::vector<std::unique_ptr<FHalfEdge>> HalfEdges;
        std::vector<std::unique_ptr<FEdge>> Edges;
        std::vector<std::unique_ptr<FVertex>> Vertices;

        int FaceIndex = 0;
        int HalfEdgeIndex = 0;
        int EdgeIndex = 0;
        int VertexIndex = 0;

        std::set<int> SelectedFaces;
        std::set<int> SelectedHalfEdges;
        std::set<int> SelectedEdges;
        std::set<int> SelectedVertices;

        std::unique_ptr<VertexArray> VertexArray_;
        std::unique_ptr<VertexArray> EdgeVertexArray_;
        std::unique_ptr<VertexArray> SelectedFaceVertexArray_;
        std::unique_ptr<VertexArray> SelectedEdgeVertexArray_;
        std::unique_ptr<VertexArray> SelectedVertexVertexArray_;

        bool bDebugNormals;
        std::unique_ptr<VertexArray> NormalsDebugVertexArray_;

        // Owner of vertex data.
        std::unique_ptr<FPositionArray> Positions;
        std::unique_ptr<FNormalArray> Normals;
        std::unique_ptr<FColorArray> Colors;
        std::unique_ptr<FTexCoordArray> TexCoords;
        std::unique_ptr<FIndexArray> Indices;

        EShadingType ShadingType;

        bool bUseImportedNormals;
    };

}
