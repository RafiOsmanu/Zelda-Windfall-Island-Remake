#include "stdafx.h"
#include "SpriteFontLoader.h"

SpriteFont* SpriteFontLoader::LoadContent(const ContentLoadInfo& loadInfo)
{
	const auto pReader = new BinaryReader();
	pReader->Open(loadInfo.assetFullPath);

	if (!pReader->Exists())
	{
		Logger::LogError(L"Failed to read the assetFile!\nPath: \'{}\'", loadInfo.assetSubPath);
		return nullptr;
	}

	TODO_W7(L"Implement SpriteFontLoader >> Parse .fnt file")
	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	char idBytes[3]{};
	for (int i = 0; i < 3; i++)
	{
		idBytes[i] = pReader->Read<char>();
	}

	//store array of char with id bits in string to check 
	std::string idString(idBytes, 3);

	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	if (idString != "BMF")
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
		return nullptr;
	}

	//Parse the version (version 3 required)
	// Read the version number from the file
	const auto version = pReader->Read<char>();

	// Check if the version is 3 or higher
	if (version < 3)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
		return nullptr;
	}

	//Valid .fnt file >> Start Parsing!
	//use this SpriteFontDesc to store all relevant information (used to initialize a SpriteFont object)
	SpriteFontDesc fontDesc{};

	//**********
	// BLOCK type 0: info *
	//**********
	//Retrieve the blockId and blockSize
	auto blockId = pReader->Read<char>();
	auto blockSize = pReader->Read<int>();


	//Retrieve the FontSize [fontDesc.fontSize]
	fontDesc.fontSize = pReader->Read<short>();
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...)]
	pReader->SetBufferPosition(23);
	//Retrieve the FontName [fontDesc.fontName]
	fontDesc.fontName = pReader->ReadNullString();

	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pReader->Read<char>();
	blockSize = pReader->Read<int>();

	pReader->SetBufferPosition(41);

	
	//Retrieve Texture Width & Height [fontDesc.textureWidth/textureHeight]
	fontDesc.textureWidth = pReader->Read<short>();
	fontDesc.textureHeight = pReader->Read<short>();
	auto pageCount = pReader->Read<short>();
	//Retrieve PageCount
	//> if pagecount > 1
	if (pageCount > 1)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Only one texture per font is allowed!");
	}
	//	> Log Error (Only one texture per font is allowed!)
	//Advance to Block2 (Move Reader)
	pReader->SetBufferPosition(52);

	

	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pReader->Read<char>();
	blockSize = pReader->Read<int>();
	//Retrieve the PageName (BinaryReader::ReadNullString)
	std::wstring pageNames = pReader->ReadNullString();
	//Construct the full path to the page texture file
	//	>> page texture should be stored next to the .fnt file, pageName contains the name of the texture file
	//	>> full texture path = asset parent_path of .fnt file (see loadInfo.assetFullPath > get parent_path) + pageName (filesystem::path::append)
	auto path = loadInfo.assetFullPath.parent_path().append(pageNames);
	//	>> Load the texture (ContentManager::Load<TextureData>) & Store [fontDesc.pTexture]
	fontDesc.pTexture = ContentManager::Load<TextureData>(path);

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pReader->Read<char>();
	blockSize = pReader->Read<int>();
	//Retrieve Character Count (see documentation)
	auto charCount = blockSize / 20;
	//Create loop for Character Count, and:
	for (int i{}; i < charCount; ++i)
	{
		//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
		auto charId = pReader->Read<UINT32>();
		
		//> Create instance of FontMetric (struct)
		FontMetric metric{};

		//	> Set Character (CharacterId) [FontMetric::character]
		metric.character = static_cast<wchar_t>(charId);

		//	> Retrieve Xposition (store Local)
		auto xPos = pReader->Read<short>();

		//	> Retrieve Yposition (store Local)
		auto yPos = pReader->Read<short>();

		//	> Retrieve & Set Width [FontMetric::width]
		metric.width = pReader->Read<short>();

		//	> Retrieve & Set Height [FontMetric::height]
		metric.height = pReader->Read<short>();

		//	> Retrieve & Set OffsetX [FontMetric::offsetX]
		metric.offsetX = pReader->Read<short>();

		//	> Retrieve & Set OffsetY [FontMetric::offsetY]
		metric.offsetY = pReader->Read<short>();

		//	> Retrieve & Set AdvanceX [FontMetric::advanceX]
		metric.advanceX = pReader->Read<short>();

		//	> Retrieve & Set Page [FontMetric::page]
		metric.page = pReader->Read<char>();

		//	> Retrieve Channel (BITFIELD!!!) 
		//		> See documentation for BitField meaning [FontMetrix::channel]
		metric.channel = pReader->Read<char>();

		//fix channel order so it matches the hlsl file
		switch (metric.channel)
		{
		case 1:
			metric.channel = 2;
			break;
		case 2:
			metric.channel = 1;
			break;
		case 4:
			metric.channel = 0;
			break;
		case 8:
			metric.channel = 3;
			break;
		default:
			metric.channel = 0;
			break;
		}

		//	> Calculate Texture Coordinates using Xposition, Yposition, fontDesc.TextureWidth & fontDesc.TextureHeight [FontMetric::texCoord]
		auto xTexCoord = xPos / static_cast<float>(fontDesc.textureWidth);
		auto yTexCoord = yPos / static_cast<float>(fontDesc.textureHeight);
		metric.texCoord = XMFLOAT2(xTexCoord, yTexCoord);
		
		//> Insert new FontMetric to the metrics [font.metrics] map
		//	> key = (wchar_t) charId
		//	> value = new FontMetric
		fontDesc.metrics.insert(std::make_pair(charId, metric));

		//(loop restarts till all metrics are parsed)
	}

	//Done!
	delete pReader;
	return new SpriteFont(fontDesc);
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
