#include "..\Inc\CRVolumetricCloudPass.h"

void VolumetricCloudPass::GetReady4Render(PassOutput * input)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GLuint couldRt;
	WindowSize size = WindowManager::Instance()->GetWindowSize();
	couldRt = SetGLRenderTexture(size.w, size.h, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
	dBuffer = SetGLDepthBuffer(size.w, size.h);

	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	output.cout = 1;
	output.RTS = new GLuint[output.cout];
	output.RTS[0] = couldRt;
}

void VolumetricCloudPass::Render(PassOutput * input)
{
	Camera cam;
	Camera* curCam = CameraManager::Instance()->GetCurrentCamera();
	cam.SetPerspectiveCamera(curCam->GetCameraPara().x, 1.0f, 100000.0f);
	cam.SetPosition(curCam->GetPosition());
	cam.SetRotation(curCam->GetRotation());
	cam.UpdateViewMatrix();
	CameraManager::Instance()->Push(cam);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, dBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, noises[0]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, noises[1]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, noises[2]);

	for (int i = 0; i < input->cout; i++)
	{
		glActiveTexture(GL_TEXTURE4 + i);
		glBindTexture(GL_TEXTURE_2D, input->RTS[i]);
	}

	float3 camPos = CameraManager::Instance()->GetCurrentCamera()->GetPosition();
	float4 sunColor = WeatherSystem::Instance()->GetSunColor();
	float4 zenithColor = WeatherSystem::Instance()->GetSkyUpColor();
	ShaderManager::Instance()->UseShader(shaderProgram);
	GLint location = glGetUniformLocation(shaderProgram, "perlinWorleyMap");
	glUniform1i(location, 1);
	location = glGetUniformLocation(shaderProgram, "worleyMap");
	glUniform1i(location, 2);
	location = glGetUniformLocation(shaderProgram, "curlMap");
	glUniform1i(location, 3);
	location = glGetUniformLocation(shaderProgram, "depthMap");
	glUniform1i(location, 4);
	location = glGetUniformLocation(shaderProgram, "wsCamPos");
	glUniform3f(location, camPos.x, camPos.y, camPos.z);
	location = glGetUniformLocation(shaderProgram, "cloudBoxSize");
	glUniform4f(location, cloudBoxScale.x, cloudBoxScale.y, cloudBoxScale.z, cloudBoxScale.w);
	location = glGetUniformLocation(shaderProgram, "sunColor");
	glUniform3f(location, sunColor.x, sunColor.y, sunColor.z);
	location = glGetUniformLocation(shaderProgram, "zenithColor");
	glUniform3f(location, zenithColor.x, zenithColor.y, zenithColor.z);

	cloudBox.Render(shaderProgram, false);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);

	for (int i = 0; i < input->cout; i++)
	{
		glActiveTexture(GL_TEXTURE4 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	CameraManager::Instance()->Pop();
}

void VolumetricCloudPass::Init()
{
	cloudBoxScale = float4(20000.0f, 1250.0f, 20000.0f, 2750.0f);
	FbxImportManager::Instance()->ImportFbxModel("Box", &cloudBox);
	cloudBox.GetReady4Rending();
	cloudBox.SetPosition(float3(0.0f, cloudBoxScale.w, 0.0f));
	cloudBox.SetScale(float3(cloudBoxScale.x, cloudBoxScale.y, cloudBoxScale.z));
	shaderProgram = ShaderManager::Instance()->LoadShader("VolumatricCloud.vert", "VolumatricCloud.frag");
	GenerateTex();
}

void VolumetricCloudPass::GenerateTex()
{
	GLubyte* perlinWorley = new GLubyte[128 * 128 * 128 * 4];
	GLubyte* worley = new GLubyte[32 * 32 * 32 * 3];
	GLubyte* curl = new GLubyte[128 * 128 * 3];
	
	bool fileExist = true;
	fstream pwNoiseFile;
	pwNoiseFile.open("Resources\\Textures\\PerlinWorley.noise", ios::in);
	if (!pwNoiseFile)
	{
		fileExist = false;
		pwNoiseFile.open("Resources\\Textures\\PerlinWorley.noise", ios::out);
	}
	for (int i = 0; i < 128; i++)
		for (int j = 0; j < 128; j++)
			for (int k = 0; k < 128; k++)
			{
				float3 uv = float3(i / 128.0f, j / 128.0f, k / 128.0f);
				if (fileExist)
				{
					float noise;
					pwNoiseFile >> noise;
					perlinWorley[i * 128 * 128 * 4 + j * 128 * 4 + k * 4 + 0] = (GLubyte)noise;

					pwNoiseFile >> noise;
					perlinWorley[i * 128 * 128 * 4 + j * 128 * 4 + k * 4 + 1] = (GLubyte)noise;

					pwNoiseFile >> noise;
					perlinWorley[i * 128 * 128 * 4 + j * 128 * 4 + k * 4 + 2] = (GLubyte)noise;

					pwNoiseFile >> noise;
					perlinWorley[i * 128 * 128 * 4 + j * 128 * 4 + k * 4 + 3] = (GLubyte)noise;
				}
				else
				{
					float noise = (Remap(Noise::PerlinFbm(uv), 0.0f, 1.0f, Noise::WorleyFbm(uv * 10.0f), 1.0f)) * 255;
					perlinWorley[i * 128 * 128 * 4 + j * 128 * 4 + k * 4 + 0] = (GLubyte)noise;
					pwNoiseFile << noise << endl;

					noise = Noise::WorleyFbm(uv * 10.0f) * 255;
					perlinWorley[i * 128 * 128 * 4 + j * 128 * 4 + k * 4 + 1] = (GLubyte)noise;
					pwNoiseFile << noise << endl;

					noise = Noise::WorleyFbm(uv * 20.0f) * 255;
					perlinWorley[i * 128 * 128 * 4 + j * 128 * 4 + k * 4 + 2] = (GLubyte)noise;
					pwNoiseFile << noise << endl;

					noise = Noise::WorleyFbm(uv * 40.0f) * 255;
					perlinWorley[i * 128 * 128 * 4 + j * 128 * 4 + k * 4 + 3] = (GLubyte)noise;
					pwNoiseFile << noise << endl;
				}
			}
	pwNoiseFile.close();

	fstream wNoiseFile;
	wNoiseFile.open("Resources\\Textures\\Worley.noise", ios::in);
	if (!wNoiseFile)
	{
		fileExist = false;
		wNoiseFile.open("Resources\\Textures\\Worley.noise", ios::out);
	}
	for (int i = 0; i < 32; i++)
		for (int j = 0; j < 32; j++)
			for (int k = 0; k < 32; k++)
			{
				float3 uv = float3(i / 32.0f, j / 32.0f, k / 32.0f);
				if (fileExist)
				{
					float noise;
					wNoiseFile >> noise;
					worley[i * 32 * 32 * 3 + j * 32 * 3 + k * 3 + 0] = (GLubyte)noise;

					wNoiseFile >> noise;
					worley[i * 32 * 32 * 3 + j * 32 * 3 + k * 3 + 1] = (GLubyte)noise;

					wNoiseFile >> noise;
					worley[i * 32 * 32 * 3 + j * 32 * 3 + k * 3 + 2] = (GLubyte)noise;
				}
				else
				{
					float noise = Noise::WorleyNoise(uv * 20.0f) * 255;
					worley[i * 32 * 32 * 3 + j * 32 * 3 + k * 3 + 0] = (GLubyte)noise;
					wNoiseFile << noise << endl;

					noise = Noise::WorleyNoise(uv * 40.0f) * 255;
					worley[i * 32 * 32 * 3 + j * 32 * 3 + k * 3 + 1] = (GLubyte)noise;
					wNoiseFile << noise << endl;

					noise = Noise::WorleyNoise(uv * 80.0f) * 255;
					worley[i * 32 * 32 * 3 + j * 32 * 3 + k * 3 + 2] = (GLubyte)noise;
					wNoiseFile << noise << endl;
				}
			}
	wNoiseFile.close();

	fstream cNoiseFile;
	for (int i = 0; i < 128; i++)
		for (int j = 0; j < 128; j++)
		{
			float3 uv = float3(i / 32.0f, j / 32.0f, 0.0f);
		}

	glGenTextures(3, noises);
	glBindTexture(GL_TEXTURE_3D, noises[0]);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 128, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, perlinWorley);

	glBindTexture(GL_TEXTURE_3D, noises[1]);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 32, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, worley);


}