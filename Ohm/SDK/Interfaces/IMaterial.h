#pragma once

#include "../Studio.h"

#include "../Math/Matrix.h"
#include "../Math/QAngle.h"
#include "../Math/Vector.h"

enum PreviewImageRetVal_t {
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};

enum MaterialVarFlags_t {
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	//MATERIAL_VAR_UNUSED = (1 << 9),
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
	MATERIAL_VAR_UNUSED = (1 << 18), // UNUSED
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_ENVMAPMODE = (1 << 25), // OBSOLETE
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
	MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
	MATERIAL_VAR_VERTEXFOG = (1 << 31),
};

typedef unsigned short ModelInstanceHandle_t;
typedef void* LightCacheHandle_t;
typedef void* StudioDecalHandle_t;
typedef int OverrideType_t;
typedef int ImageFormat;
typedef int VertexFormat_t;
typedef int MaterialPropertyTypes_t;

class IClientRenderable;
class CStudioHdr;
class IMatRenderContext;
class DataCacheHandle_t;
class ITexture;
class IMaterialVar;
class KeyValues;
struct mstudioanimdesc_t;
struct mstudioseqdesc_t;
struct Ray_t;
struct DrawModelInfo_t;
struct studiohwdata_t;
struct MaterialLightingState_t;
struct ColorMeshInfo_t;

class studiohdr_t;

struct DrawModelState_t {
	studiohdr_t* m_pStudioHdr;
	studiohwdata_t* m_pStudioHWData;
	IClientRenderable* m_pRenderable;
	const matrix3x4_t* m_pModelToWorld;
	StudioDecalHandle_t     m_decals;
	int                     m_drawFlags;
	int                     m_lod;
};

struct StaticPropRenderInfo_t {
	const matrix3x4_t* pModelToWorld;
	const model_t* pModel;
	IClientRenderable* pRenderable;
	Vector* pLightingOrigin;
	short                   skin;
	ModelInstanceHandle_t   instance;
};

struct ModelRenderInfo_t {
	Vector                  origin;
	QAngle                  angles;
	char                    pad[4];
	IClientRenderable* pRenderable;
	const model_t* pModel;
	const matrix3x4_t* pModelToWorld;
	const matrix3x4_t* pLightingOffset;
	const Vector* pLightingOrigin;
	int                     flags;
	int                     entity_index;
	int                     skin;
	int                     body;
	int                     hitboxset;
	ModelInstanceHandle_t   instance;

	ModelRenderInfo_t() {
		pModelToWorld = 0;
		pLightingOffset = NULL;
		pLightingOrigin = NULL;
	}
};

struct LightingQuery_t {
	Vector                  m_LightingOrigin;
	ModelInstanceHandle_t   m_InstanceHandle;
	bool                    m_bAmbientBoost;
};

struct StaticLightingQuery_t : public LightingQuery_t {
	IClientRenderable* m_pRenderable;
};

class IMaterial {
public:
	virtual const char* GetName() const = 0;
	virtual const char* GetTextureGroupName() const = 0;
	virtual PreviewImageRetVal_t    GetPreviewImageProperties(int* width, int* height, ImageFormat* imageFormat, bool* isTranslucent) const = 0;
	virtual PreviewImageRetVal_t    GetPreviewImage(unsigned char* data, int width, int height, ImageFormat imageFormat) const = 0;
	virtual int                     GetMappingWidth() = 0;
	virtual int                     GetMappingHeight() = 0;
	virtual int                     GetNumAnimationFrames() = 0;
	virtual bool                    InMaterialPage(void) = 0;
	virtual    void                 GetMaterialOffset(float* pOffset) = 0;
	virtual void                    GetMaterialScale(float* pScale) = 0;
	virtual IMaterial* GetMaterialPage(void) = 0;
	virtual IMaterialVar* FindVar(const char* varName, bool* found, bool complain = true) = 0;
	virtual void                    IncrementReferenceCount(void) = 0;
	virtual void                    DecrementReferenceCount(void) = 0;
	inline void                     AddRef() { IncrementReferenceCount(); }
	inline void                     Release() { DecrementReferenceCount(); }
	virtual int                     GetEnumerationID(void) const = 0;
	virtual void                    GetLowResColorSample(float s, float t, float* color) const = 0;
	virtual void                    RecomputeStateSnapshots() = 0;
	virtual bool                    IsTranslucent() = 0;
	virtual bool                    IsAlphaTested() = 0;
	virtual bool                    IsVertexLit() = 0;
	virtual VertexFormat_t          GetVertexFormat() const = 0;
	virtual bool                    HasProxy(void) const = 0;
	virtual bool                    UsesEnvCubemap(void) = 0;
	virtual bool                    NeedsTangentSpace(void) = 0;
	virtual bool                    NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool                    NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool                    NeedsSoftwareSkinning(void) = 0;
	virtual void                    AlphaModulate(float alpha) = 0;
	virtual void                    ColorModulate(float r, float g, float b) = 0;
	virtual void                    SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
	virtual bool                    GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;
	virtual void                    GetReflectivity(Vector& reflect) = 0;
	virtual bool                    GetPropertyFlag(MaterialPropertyTypes_t type) = 0;
	virtual bool                    IsTwoSided() = 0;
	virtual void                    SetShader(const char* pShaderName) = 0;
	virtual int                     GetNumPasses(void) = 0;
	virtual int                     GetTextureMemoryBytes(void) = 0;
	virtual void                    Refresh() = 0;
	virtual bool                    NeedsLightmapBlendAlpha(void) = 0;
	virtual bool                    NeedsSoftwareLighting(void) = 0;
	virtual int                     ShaderParamCount() const = 0;
	virtual IMaterialVar** GetShaderParams(void) = 0;
	virtual bool                    IsErrorMaterial() const = 0;
	virtual void                    Unused() = 0;
	virtual float                   GetAlphaModulation() = 0;
	virtual void                    GetColorModulation(float* r, float* g, float* b) = 0;
	virtual bool                    IsTranslucentUnderModulation(float fAlphaModulation = 1.0f) const = 0;
	virtual IMaterialVar* FindVarFast(char const* pVarName, unsigned int* pToken) = 0;
	virtual void                    SetShaderAndParams(KeyValues* pKeyValues) = 0;
	virtual const char* GetShaderName() const = 0;
	virtual void                    DeleteIfUnreferenced() = 0;
	virtual bool                    IsSpriteCard() = 0;
	virtual void                    CallBindProxy(void* proxyData) = 0;
	virtual void                    RefreshPreservingMaterialVars() = 0;
	virtual bool                    WasReloadedFromWhitelist() = 0;
	virtual bool                    SetTempExcluded(bool bSet, int nExcludedDimensionLimit) = 0;
	virtual int                     GetReferenceCount() const = 0;
};