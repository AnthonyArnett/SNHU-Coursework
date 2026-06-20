///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationX * rotationY * rotationZ * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/

void SceneManager::DefineObjectMaterials()
{
	OBJECT_MATERIAL defaultMaterial;
	defaultMaterial.ambientColor = glm::vec3(0.15f, 0.15f, 0.15f);
	defaultMaterial.ambientStrength = 0.20f;
	defaultMaterial.diffuseColor = glm::vec3(0.75f, 0.75f, 0.75f);
	defaultMaterial.specularColor = glm::vec3(0.25f, 0.25f, 0.25f);
	defaultMaterial.shininess = 16.0f;
	defaultMaterial.tag = "default";
	m_objectMaterials.push_back(defaultMaterial);

	// Shiny material for monitor screens
	OBJECT_MATERIAL screenMaterial;
	screenMaterial.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	screenMaterial.ambientStrength = 0.2f;
	screenMaterial.diffuseColor = glm::vec3(0.8f, 0.8f, 0.8f);
	screenMaterial.specularColor = glm::vec3(0.9f, 0.9f, 0.9f);
	screenMaterial.shininess = 64.0f;
	screenMaterial.tag = "screen";
	m_objectMaterials.push_back(screenMaterial);
}

void SceneManager::SetupSceneLights()
{
	m_pShaderManager->setBoolValue(g_UseLightingName, true);

	// Light 0 -
	m_pShaderManager->setVec3Value("lightSources[0].position", glm::vec3(0.0f, 8.0f, 6.0f));
	m_pShaderManager->setVec3Value("lightSources[0].ambientColor", glm::vec3(0.18f, 0.18f, 0.18f));
	m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", glm::vec3(0.75f, 0.70f, 0.62f));
	m_pShaderManager->setVec3Value("lightSources[0].specularColor", glm::vec3(0.25f, 0.25f, 0.25f));
	m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 32.0f);
	m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 0.10f);

	// Light 1 -
	m_pShaderManager->setVec3Value("lightSources[1].position", glm::vec3(7.0f, 3.0f, 1.5f));
	m_pShaderManager->setVec3Value("lightSources[1].ambientColor", glm::vec3(0.08f, 0.03f, 0.12f));
	m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", glm::vec3(0.35f, 0.12f, 0.45f));
	m_pShaderManager->setVec3Value("lightSources[1].specularColor", glm::vec3(0.20f, 0.08f, 0.30f));
	m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 16.0f);
	m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.06f);

	for (int i = 2; i < 4; i++)
	{
		m_pShaderManager->setVec3Value("lightSources[" + std::to_string(i) + "].position", glm::vec3(0.0f));
		m_pShaderManager->setVec3Value("lightSources[" + std::to_string(i) + "].ambientColor", glm::vec3(0.0f));
		m_pShaderManager->setVec3Value("lightSources[" + std::to_string(i) + "].diffuseColor", glm::vec3(0.0f));
		m_pShaderManager->setVec3Value("lightSources[" + std::to_string(i) + "].specularColor", glm::vec3(0.0f));
		m_pShaderManager->setFloatValue("lightSources[" + std::to_string(i) + "].focalStrength", 1.0f);
		m_pShaderManager->setFloatValue("lightSources[" + std::to_string(i) + "].specularIntensity", 0.0f);
	}
}


/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene

	DefineObjectMaterials();
	SetupSceneLights();

	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadTorusMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadSphereMesh();

	// Load in the textures
	CreateGLTexture("../../Utilities/textures/Wood.jpg", "wood");
	CreateGLTexture("../../Utilities/textures/MugHandle.jpg", "mug_handle");
	CreateGLTexture("../../Utilities/textures/MugBody.jpg", "mug_body");
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	BindGLTextures();

	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	// Desk - thin box used instead of a plane so the front edge looks straight
	scaleXYZ = glm::vec3(9.5f, 0.08f, 6.5f);
	positionXYZ = glm::vec3(0.0f, -0.04f, 0.0f);

	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("wood");
	SetTextureUVScale(4.0f, 4.0f);
	SetShaderMaterial("default");
	m_basicMeshes->DrawBoxMesh();

	// Mug (Body) - Cylinder scaled and translated to the proper position on the desk

	scaleXYZ = glm::vec3(0.45f, 0.75f, 0.45f);
	positionXYZ = glm::vec3(-3.4f, 0.08f, 2.25f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("mug_body");
	SetTextureUVScale(0.25f, 0.25f);
	// SetShaderColor(0.95f, 0.90f, 0.80f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();


	// Mug Handle - Torus scaled and translated to the proper position on the desk
	scaleXYZ = glm::vec3(0.22f, 0.30f, 0.22f);
	positionXYZ = glm::vec3(-3.85f, 0.48f, 2.25f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("mug_handle");
	SetTextureUVScale(1.0f, 1.0f);
	//SetShaderColor(0.85f, 0.15f, 0.20f, 1.0f);
	m_basicMeshes->DrawTorusMesh();

	// Left Monitor Bezel
	scaleXYZ = glm::vec3(4.6f, 2.6f, 0.12f);
	positionXYZ = glm::vec3(-2.4f, 2.20f, -2.0f);
	SetTransformations(scaleXYZ, 0.0f, 12.0f, 0.0f, positionXYZ);
	SetShaderColor(0.02f, 0.02f, 0.02f, 1.0f);
	SetShaderMaterial("default");
	m_basicMeshes->DrawBoxMesh();

	// Left Monitor Screen
	scaleXYZ = glm::vec3(4.35f, 2.35f, 0.08f);
	positionXYZ = glm::vec3(-2.4f, 2.22f, -1.88f);
	SetTransformations(scaleXYZ, 0.0f, 12.0f, 0.0f, positionXYZ);
	SetShaderColor(0.05f, 0.05f, 0.06f, 1.0f);
	SetShaderMaterial("screen");
	m_basicMeshes->DrawBoxMesh();

	// Left Monitor Neck (thin vertical pole)
	scaleXYZ = glm::vec3(0.12f, 1.0f, 0.12f);
	positionXYZ = glm::vec3(-2.4f, 0.55f, -2.1f);
	SetTransformations(scaleXYZ, 0.0f, 12.0f, 0.0f, positionXYZ);
	SetShaderColor(0.08f, 0.08f, 0.08f, 1.0f);
	SetShaderMaterial("default");
	m_basicMeshes->DrawBoxMesh();

	// Left Monitor Base (wide flat oval-ish box)
	scaleXYZ = glm::vec3(1.4f, 0.06f, 0.7f);
	positionXYZ = glm::vec3(-2.4f, 0.07f, -1.8f);
	SetTransformations(scaleXYZ, 0.0f, 12.0f, 0.0f, positionXYZ);
	SetShaderColor(0.08f, 0.08f, 0.08f, 1.0f);
	SetShaderMaterial("default");
	m_basicMeshes->DrawBoxMesh();


	// Right Monitor Bezel
	scaleXYZ = glm::vec3(4.6f, 2.6f, 0.12f);
	positionXYZ = glm::vec3(2.4f, 2.20f, -2.0f);
	SetTransformations(scaleXYZ, 0.0f, -12.0f, 0.0f, positionXYZ);
	SetShaderColor(0.02f, 0.02f, 0.02f, 1.0f);
	SetShaderMaterial("default");
	m_basicMeshes->DrawBoxMesh();

	// Right Monitor Screen
	scaleXYZ = glm::vec3(4.35f, 2.35f, 0.08f);
	positionXYZ = glm::vec3(2.4f, 2.22f, -1.88f);
	SetTransformations(scaleXYZ, 0.0f, -12.0f, 0.0f, positionXYZ);
	SetShaderColor(0.05f, 0.05f, 0.06f, 1.0f);
	SetShaderMaterial("screen");
	m_basicMeshes->DrawBoxMesh();

	// Right Monitor Neck
	scaleXYZ = glm::vec3(0.12f, 1.0f, 0.12f);
	positionXYZ = glm::vec3(2.4f, 0.55f, -2.1f);
	SetTransformations(scaleXYZ, 0.0f, -12.0f, 0.0f, positionXYZ);
	SetShaderColor(0.08f, 0.08f, 0.08f, 1.0f);
	SetShaderMaterial("default");
	m_basicMeshes->DrawBoxMesh();

	// Right Monitor Base
	scaleXYZ = glm::vec3(1.4f, 0.06f, 0.7f);
	positionXYZ = glm::vec3(2.4f, 0.07f, -1.8f);
	SetTransformations(scaleXYZ, 0.0f, -12.0f, 0.0f, positionXYZ);
	SetShaderColor(0.08f, 0.08f, 0.08f, 1.0f);
	SetShaderMaterial("default");
	m_basicMeshes->DrawBoxMesh();

	// Keyboard Base
	scaleXYZ = glm::vec3(3.5f, 0.08f, 1.0f);
	positionXYZ = glm::vec3(0.5f, 0.04f, 1.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.08f, 0.08f, 0.08f, 1.0f);
	SetShaderMaterial("default");
	m_basicMeshes->DrawBoxMesh();

	// Keyboard Keys
	scaleXYZ = glm::vec3(3.15f, 0.03f, 0.80f);
	positionXYZ = glm::vec3(0.5f, 0.10f, 1.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.15f, 0.15f, 0.15f, 1.0f);
	SetShaderMaterial("default");
	m_basicMeshes->DrawBoxMesh();

	// Escape Key
	scaleXYZ = glm::vec3(0.22f, 0.02f, 0.18f);
	positionXYZ = glm::vec3(-0.95f, 0.125f, 0.62f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.22f, 0.22f, 0.22f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// Enter Key
	scaleXYZ = glm::vec3(0.35f, 0.02f, 0.18f);
	positionXYZ = glm::vec3(1.35f, 0.125f, 0.62f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.22f, 0.22f, 0.22f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// Spacebar
	scaleXYZ = glm::vec3(1.30f, 0.02f, 0.15f);
	positionXYZ = glm::vec3(0.5f, 0.125f, 1.25f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.22f, 0.22f, 0.22f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// Mouse base
	scaleXYZ = glm::vec3(0.50f, 0.13f, 0.70f);
	positionXYZ = glm::vec3(3.2f, 0.08f, 1.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.08f, 0.08f, 0.08f, 1.0f);
	SetShaderMaterial("screen");
	m_basicMeshes->DrawBoxMesh();

	// Mouse top
	scaleXYZ = glm::vec3(0.42f, 0.05f, 0.55f);
	positionXYZ = glm::vec3(3.2f, 0.17f, 1.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.13f, 0.13f, 0.13f, 1.0f);
	SetShaderMaterial("screen");
	m_basicMeshes->DrawBoxMesh();

	// Mouse left button
	scaleXYZ = glm::vec3(0.18f, 0.02f, 0.28f);
	positionXYZ = glm::vec3(3.08f, 0.205f, 0.78f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.20f, 0.20f, 0.20f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// Mouse right button
	scaleXYZ = glm::vec3(0.18f, 0.02f, 0.28f);
	positionXYZ = glm::vec3(3.32f, 0.205f, 0.78f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.20f, 0.20f, 0.20f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// Mouse scroll wheel
	scaleXYZ = glm::vec3(0.05f, 0.05f, 0.10f);
	positionXYZ = glm::vec3(3.2f, 0.22f, 0.78f);
	SetTransformations(scaleXYZ, 90.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.02f, 0.02f, 0.02f, 1.0f);
	SetShaderMaterial("default");
	m_basicMeshes->DrawCylinderMesh();

	// Light Marker
	scaleXYZ = glm::vec3(0.25f, 0.25f, 0.25f);
	positionXYZ = glm::vec3(-4.0f, 0.35f, -2.7f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(1.0f, 0.85f, 0.25f, 1.0f);
	m_basicMeshes->DrawSphereMesh();

	// Light Marker 2 (Purple Side Light)
	scaleXYZ = glm::vec3(0.25f, 0.25f, 0.25f);
	positionXYZ = glm::vec3(4.0f, 0.35f, -2.7f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.65f, 0.20f, 1.0f, 1.0f);
	m_basicMeshes->DrawSphereMesh();
}
