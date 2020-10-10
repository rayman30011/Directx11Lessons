#pragma once
#include "BaseApp.h"
#include "GeometryGenerator.h"
#include "DynamicMesh.h"
#include "LightHelper.h"

class App : public BaseApp
{
public:
	App();

	void init(HINSTANCE hinst) override;
	void update(float time) override;
	void render() override;

	void onMouseMove(WPARAM wparam, uint32_t, uint32_t) override;
	void onMouseWheel(short value) override;
private:
	void buildFx();
	void buildGeomerty();
	void buildInputLayout();

	void renderMesh(Mesh& mesh) const;

private:
	GeometryGenerator generator;

	ID3DX11Effect* _effect;
	ID3DX11EffectTechnique* _tech; 
	ID3DX11EffectMatrixVariable* _fxWorldViewProj;
	ID3DX11EffectMatrixVariable* _fxWorldInv;
	ID3DX11EffectMatrixVariable* _fxWorld;
	ID3DX11EffectMatrixVariable* _fxTextureTransform;
	ID3DX11EffectVariable* _fxDirectionLight;
	ID3DX11EffectVariable* _fxSpotLight;
	ID3DX11EffectVariable* _fxPointLight;
	ID3DX11EffectVectorVariable* _fxEyePosition;
	ID3DX11EffectVariable* _fxMaterial;
	ID3DX11EffectShaderResourceVariable* _fxDiffuseMap;
	
	ID3D11ShaderResourceView* _textureSRW;
	
	XMFLOAT4X4 _worldMatrix;
	XMFLOAT4X4 _viewMatrix;
	XMFLOAT4X4 _projMatrix;
	XMFLOAT4X4 _textureTransform;

	Mesh _box;
	Mesh _terrain;
	Mesh _cylinder;
	Mesh _sphere;
	DynamicMesh _wawes;

	DirectionLight _directionLight;
	PointLight _pointLight;
	SpotLight _spotLight;

	std::vector<XMFLOAT3> _cylinderPositions;

	XMFLOAT3 _cameraPosition;

	float _angle;
	float _theta; 
	float _phi; 
	float _radius;
};

