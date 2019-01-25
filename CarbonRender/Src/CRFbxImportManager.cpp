#include "..\Inc\CRFbxImportManager.h"

FbxImportManager* FbxImportManager::ins = nullptr;

FbxImportManager::FbxImportManager()
{
	Init();
}

FbxImportManager::~FbxImportManager()
{
	fbxManager->Destroy();
	ioSettings->Destroy();
	ins = nullptr;
}

FbxImportManager * FbxImportManager::Instance()
{
	if (ins == nullptr)
		ins = new FbxImportManager();

	return ins;
}

void FbxImportManager::Init()
{
	fbxManager = FbxManager::Create();
	ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ioSettings);
}

FbxColor FbxImportManager::ReadColor(FbxMesh * mesh, int index, int vertexID)
{
	if (mesh->GetElementVertexColorCount() <= 0)
		return FbxColor(0.0f, 0.0f, 0.0f, 1.0f);

	FbxGeometryElementVertexColor* eVertexColor = mesh->GetElementVertexColor(0);
	FbxColor color;
	switch (eVertexColor->GetMappingMode())
	{
	default:
		break;
	case FbxGeometryElement::eByControlPoint:
	{
		switch (eVertexColor->GetReferenceMode())
		{
		default:
			break;
		case FbxGeometryElement::eDirect:
			color = eVertexColor->GetDirectArray().GetAt(index);
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = eVertexColor->GetIndexArray().GetAt(index);
			color = eVertexColor->GetDirectArray().GetAt(id);
		}
		break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (eVertexColor->GetReferenceMode())
		{
		default:
			break;
		case FbxGeometryElement::eDirect:
			color = eVertexColor->GetDirectArray().GetAt(vertexID);
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = eVertexColor->GetIndexArray().GetAt(vertexID);
			color = eVertexColor->GetDirectArray().GetAt(id);
		}
		break;
		}
	}
	break;
	}
	return color;
}

float4 FbxImportManager::ReadUV(FbxMesh * mesh, int index, int uvIndex)
{
	float4 uvf(-1.0f, -1.0f, -1.0f, -1.0f);
	int elementCount = mesh->GetElementUVCount();
	if (elementCount <= 0)
		return uvf;

	float uva[4] = { -1.0f, -1.0f, -1.0f, -1.0f };

	for (int i = 0; i < elementCount; i++)
	{
		FbxGeometryElementUV* eUV = mesh->GetElementUV(i);
		FbxVector2 uv;

		switch (eUV->GetMappingMode())
		{
		default:
			break;
		case FbxGeometryElement::eByControlPoint:
		{
			switch (eUV->GetReferenceMode())
			{
			default:
				break;
			case FbxGeometryElement::eDirect:
				uv = eUV->GetDirectArray().GetAt(index);
				break;
			case FbxGeometryElement::eIndexToDirect:
			{
				int id = eUV->GetIndexArray().GetAt(index);
				uv = eUV->GetDirectArray().GetAt(id);
			}
			break;
			}
		}
		break;
		case FbxGeometryElement::eByPolygonVertex:
		{

			switch (eUV->GetReferenceMode())
			{
			default:
				break;
			case FbxGeometryElement::eDirect:
			case FbxGeometryElement::eIndexToDirect:
				uv = eUV->GetDirectArray().GetAt(uvIndex);
				break;
			}
		}
		break;
		}

		uva[i * 2] = uv[0];
		uva[i * 2 + 1] = uv[1];
	}

	uvf.x = uva[0];
	uvf.y = uva[1];
	uvf.z = uva[2];
	uvf.w = uva[3];

	return uvf;
}

FbxVector4 FbxImportManager::ReadNormal(FbxMesh * mesh, int index, int vertexID)
{
	FbxVector4 n(0.0f, 0.0f, 1.0f, 1.0f);
	if (mesh->GetElementNormalCount() <= 0)
		return n;

	FbxGeometryElementNormal* eN = mesh->GetElementNormal(0);
	switch (eN->GetMappingMode())
	{
	default:
		break;
	case FbxGeometryElement::eByControlPoint:
	{
		switch (eN->GetReferenceMode())
		{
		default:
			break;
		case FbxGeometryElement::eDirect:
			n = eN->GetDirectArray().GetAt(index);
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = eN->GetIndexArray().GetAt(index);
			n = eN->GetDirectArray().GetAt(id);
		}
		break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (eN->GetReferenceMode())
		{
		default:
			break;
		case FbxGeometryElement::eDirect:
			n = eN->GetDirectArray().GetAt(vertexID);
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = eN->GetIndexArray().GetAt(vertexID);
			n = eN->GetDirectArray().GetAt(id);
		}
		break;
		}
	}
	break;
	}

	return n;
}

FbxVector4 FbxImportManager::ReadTangent(FbxMesh * mesh, int index, int vertexID)
{
	FbxVector4 t(1.0f, 0.0f, 0.0f, 1.0f);
	if (mesh->GetElementTangentCount() <= 0)
		return t;

	FbxGeometryElementTangent* eT = mesh->GetElementTangent(0);
	switch (eT->GetMappingMode())
	{
	default:
		break;
	case FbxGeometryElement::eByControlPoint:
	{
		switch (eT->GetReferenceMode())
		{
		default:
			break;
		case FbxGeometryElement::eDirect:
			t = eT->GetDirectArray().GetAt(index);
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = eT->GetIndexArray().GetAt(index);
			t = eT->GetDirectArray().GetAt(id);
		}
		break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (eT->GetReferenceMode())
		{
		default:
			break;
		case FbxGeometryElement::eDirect:
			t = eT->GetDirectArray().GetAt(vertexID);
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = eT->GetIndexArray().GetAt(vertexID);
			t = eT->GetDirectArray().GetAt(id);
		}
		break;
		}
	}
	break;
	}

	return t;
}

FbxVector4 FbxImportManager::ReadBinormal(FbxMesh * mesh, int index, int vertexID)
{
	FbxVector4 b(1.0f, 0.0f, 0.0f, 1.0f);
	if (mesh->GetElementBinormalCount() <= 0)
		return b;

	FbxGeometryElementBinormal* eB = mesh->GetElementBinormal(0);
	switch (eB->GetMappingMode())
	{
	default:
		break;
	case FbxGeometryElement::eByControlPoint:
	{
		switch (eB->GetReferenceMode())
		{
		default:
			break;
		case FbxGeometryElement::eDirect:
			b = eB->GetDirectArray().GetAt(index);
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = eB->GetIndexArray().GetAt(index);
			b = eB->GetDirectArray().GetAt(id);
		}
		break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (eB->GetReferenceMode())
		{
		default:
			break;
		case FbxGeometryElement::eDirect:
			b = eB->GetDirectArray().GetAt(vertexID);
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = eB->GetIndexArray().GetAt(vertexID);
			b = eB->GetDirectArray().GetAt(id);
		}
		break;
		}
	}
	break;
	}

	return b;
}

void FbxImportManager::ReadTexture(FbxMesh* mesh, MeshObject* meshObj, char* meshFileName)
{
	if (mesh->GetNode()->GetMaterialCount() > 0)
	{
		//surport just 1 material on 1 mesh for now
		FbxSurfaceMaterial* mat = mesh->GetNode()->GetMaterial(0);
		FbxProperty p;

		p = mat->FindProperty(FbxLayerElement::sTextureChannelNames[0]);
		if (p.IsValid())
		{
			int texCount = p.GetSrcObjectCount<FbxTexture>();
			for (int k = 0; k < texCount; k++)
			{
				FbxTexture* fbxTex = FbxCast<FbxTexture>(p.GetSrcObject<FbxTexture>(k));
				if (fbxTex && strlen(fbxTex->GetInitialName()) != 0)
				{
					char* fullName = FileReader::BindString(meshFileName, "\\");
					fullName = FileReader::BindString(fullName, (char*)fbxTex->GetInitialName());
					if (strstr(fbxTex->GetInitialName(), "_D"))
						meshObj->GetMaterial()->SetDiffuse(fullName);
					if (strstr(fbxTex->GetInitialName(), "_N"))
						meshObj->GetMaterial()->SetNormal(fullName);
					if (strstr(fbxTex->GetInitialName(), "_S"))
						meshObj->GetMaterial()->SetSpecular(fullName);
				}
			}

			if (meshObj->GetMaterial()->GetDiffuse() == 0)
				meshObj->GetMaterial()->SetDiffuse("");
			if (meshObj->GetMaterial()->GetNormal() == 0)
				meshObj->GetMaterial()->SetNormal("");
			if (meshObj->GetMaterial()->GetSpecular() == 0)
				meshObj->GetMaterial()->SetSpecular("");

			return;
		}
	}

	meshObj->GetMaterial()->SetDiffuse("");
	meshObj->GetMaterial()->SetNormal("");
	meshObj->GetMaterial()->SetSpecular("");
}

int FbxImportManager::ImportFbxModel(char * fileName, Object* root, bool loadTex)
{
	root->SetName(fileName);
	char* dir = "Resources\\Models\\";
	char* fullName = FileReader::BindString(dir, fileName);
	fullName = FileReader::BindString(fullName, ".fbx");
	FbxImporter* importer = FbxImporter::Create(this->fbxManager, "");
	if (!importer->Initialize(fullName, -1, this->fbxManager->GetIOSettings()))
	{
		cout << "Fbx Importer init fail" << endl;
		cout << "Error Log: " << importer->GetStatus().GetErrorString();
		return -1;
	}

	FbxScene* scene = FbxScene::Create(this->fbxManager, fullName);
	importer->Import(scene);
	importer->Destroy();

	FbxNode* rootNode = scene->GetRootNode();
	int meshCount = scene->GetGeometryCount();
	int skeletonCount = rootNode->GetChildCount() - meshCount;

	if (rootNode != NULL)
	{
		int readMeshCount = 0;
		for (int nodeIndex = 0; nodeIndex < rootNode->GetChildCount(); nodeIndex++)
		{
			FbxNode* node = rootNode->GetChild(nodeIndex);

			if (node->GetNodeAttribute())
			{
				switch (node->GetNodeAttribute()->GetAttributeType())
				{
				default:
					break;
				case FbxNodeAttribute::eMesh:
				{
					FbxDouble3 translation = node->LclTranslation.Get();
					FbxDouble3 rotation = node->LclRotation.Get();
					FbxDouble3 scaling = node->LclScaling.Get();

					FbxMesh* mesh = node->GetMesh();
					MeshObject* newMeshObj = new MeshObject();
					int ctrlPointsCount = mesh->GetControlPointsCount();
					newMeshObj->SetName(mesh->GetName());
					newMeshObj->SetPath(fileName, mesh->GetName());
					newMeshObj->SetPosition(translation);
					newMeshObj->SetRotation(rotation);
					newMeshObj->SetScale(scaling);
					//newMeshObj.modelMatrix = Math::CalculateModelMatrix(newMeshObj.localCoord, translation, rotation, scaling);
					newMeshObj->SetPolygonCount(mesh->GetPolygonCount());
					newMeshObj->SetVertexCount(newMeshObj->GetPolygonCount() * 3);
					newMeshObj->CreateIndexArray(newMeshObj->GetPolygonCount() * 3);

					bool* mark = new bool[ctrlPointsCount];
					for (int i = 0; i < ctrlPointsCount; i++)
						mark[i] = false;
					int* multiNormalIndex = new int[ctrlPointsCount * 30];
					float* tempVertexArray = new float[newMeshObj->GetVertexCount() * 3];
					float* tempColorArray = new float[newMeshObj->GetVertexCount() * 4];
					float* tempUVArray = new float[newMeshObj->GetVertexCount() * 4];
					float* tempNormalArray = new float[newMeshObj->GetVertexCount() * 3];
					float* tempTangentArray = new float[newMeshObj->GetVertexCount() * 3];
					float* tempBinormalArray = new float[newMeshObj->GetVertexCount() * 3];

					//Get Textures
					newMeshObj->SetMaterial(MaterialManager::Instance()->CreateNewMaterial());
					if (loadTex)
						ReadTexture(mesh, newMeshObj, fileName);
					else
					{
						newMeshObj->GetMaterial()->SetDiffuse("");
						newMeshObj->GetMaterial()->SetNormal("");
						newMeshObj->GetMaterial()->SetSpecular("");
					}

					int addVertexCount = 0;
					FbxDouble4* ctrlPoints = mesh->GetControlPoints();
					for (int polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); polygonIndex++)
					{
						for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++)
						{
							int vertexID = polygonIndex * 3 + vertexIndex;
							unsigned int index = (unsigned int)mesh->GetPolygonVertex(polygonIndex, vertexIndex);
							newMeshObj->SetIndexAt(vertexID, index);

							//Get Vertex
							float3 v(ctrlPoints[index][0], ctrlPoints[index][1], ctrlPoints[index][2]);

							//Get Color
							FbxColor color = ReadColor(mesh, index, vertexID);

							//Get UV
							int uvIndex = mesh->GetTextureUVIndex(polygonIndex, vertexIndex);
							float4 uv = ReadUV(mesh, index, uvIndex);

							//Get Normal
							float3 n = ReadNormal(mesh, index, vertexID);

							//Get Tangent
							float3 t = ReadTangent(mesh, index, vertexID);

							//Get Binormal
							float3 b = ReadBinormal(mesh, index, vertexID);

							int finalIndex = index;
							if (!mark[index])
							{
								mark[index] = true;
								multiNormalIndex[index * 30] = 1;
								multiNormalIndex[index * 30 + 1] = index;
							}
							else
							{
								bool hasSame = false;
								int sameIndex = 0;
								for (int i = 1; i <= multiNormalIndex[index * 30]; i++)
								{
									int sampleIndex = multiNormalIndex[index * 30 + i];
									float nDiff = Math::Distance(n, float3(tempNormalArray[sampleIndex * 3],
										tempNormalArray[sampleIndex * 3 + 1],
										tempNormalArray[sampleIndex * 3 + 2]));
									float uvDiff = Math::Distance(uv, float4(tempUVArray[sampleIndex * 4],
										tempUVArray[sampleIndex * 4 + 1],
										tempUVArray[sampleIndex * 4 + 2],
										tempUVArray[sampleIndex * 4 + 3]));

									if (nDiff == 0.0f && uvDiff == 0.0f)
									{
										hasSame = true;
										sameIndex = sampleIndex;
										break;
									}
								}

								if (!hasSame)
								{
									finalIndex = ctrlPointsCount + addVertexCount;
									newMeshObj->SetIndexAt(vertexID, finalIndex);

									multiNormalIndex[index * 30]++;
									multiNormalIndex[index * 30 + multiNormalIndex[index * 30]] = finalIndex;
									addVertexCount++;
								}
								else
								{
									newMeshObj->SetIndexAt(vertexID, sameIndex);
									continue;
								}
							}

							tempVertexArray[finalIndex * 3] = v.x;
							tempVertexArray[finalIndex * 3 + 1] = v.y;
							tempVertexArray[finalIndex * 3 + 2] = v.z;

							tempColorArray[finalIndex * 4] = color.mRed;
							tempColorArray[finalIndex * 4 + 1] = color.mGreen;
							tempColorArray[finalIndex * 4 + 2] = color.mBlue;
							tempColorArray[finalIndex * 4 + 3] = color.mAlpha;

							tempUVArray[finalIndex * 4] = uv.x;
							tempUVArray[finalIndex * 4 + 1] = uv.y;
							tempUVArray[finalIndex * 4 + 2] = uv.z;
							tempUVArray[finalIndex * 4 + 3] = uv.w;

							tempNormalArray[finalIndex * 3] = n.x;
							tempNormalArray[finalIndex * 3 + 1] = n.y;
							tempNormalArray[finalIndex * 3 + 2] = n.z;

							tempTangentArray[finalIndex * 3] = t.x;
							tempTangentArray[finalIndex * 3 + 1] = t.y;
							tempTangentArray[finalIndex * 3 + 2] = t.z;

							tempBinormalArray[finalIndex * 3] = b.x;
							tempBinormalArray[finalIndex * 3 + 1] = b.y;
							tempBinormalArray[finalIndex * 3 + 2] = b.z;
						}
					}

					newMeshObj->SetVertexCount(ctrlPointsCount + addVertexCount);
					newMeshObj->CreateVertexArray(newMeshObj->GetVertexCount() * 3);
					newMeshObj->CreateVertexColorArray(newMeshObj->GetVertexCount() * 4);
					newMeshObj->CreateUVArray(newMeshObj->GetVertexCount() * 4);
					newMeshObj->CreateNormalArray(newMeshObj->GetVertexCount() * 3);
					newMeshObj->CreateTangentArray(newMeshObj->GetVertexCount() * 3);
					newMeshObj->CreateBinormalArray(newMeshObj->GetVertexCount() * 3);
					newMeshObj->CopyToVertexArray(tempVertexArray);
					newMeshObj->CopyToVertexColorArray(tempColorArray);
					newMeshObj->CopyToUVArray(tempUVArray);
					newMeshObj->CopyToNormalrray(tempNormalArray);
					newMeshObj->CopyToTangentArray(tempTangentArray);
					newMeshObj->CopyToBinormalArray(tempBinormalArray);

					newMeshObj->GetReady4Rending();
					root->AddChild((Object*)newMeshObj);
					
  					readMeshCount++;
				}
				break;
				case FbxNodeAttribute::eSkeleton:
				{

				}
				break;
				}
			}
		}
	}

	return meshCount;
}

int FbxImportManager::ImportFbxModel(MeshObject * meshObj)
{
	return 0;
}
