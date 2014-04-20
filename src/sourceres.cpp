/*
 *  sourceres.cpp
 *  SourceRes project
 *  
 *  Copyright (c) 2013 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#include "sourceres.h"

/*
 * Console commands
 */
static void sr_forceres( const CCommand& args );
static ConCommand sr_forceres_cmd("sr_forceres", sr_forceres, "Force TF2 to run at a certain windowed resolution");

// The plugin is a static singleton that is exported as an interface
SequencePlugin g_SequencePlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(SequencePlugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_SequencePlugin );

/*
 * SequencePlugin implementation
 * 
 */
SequencePlugin::SequencePlugin(){}
SequencePlugin::~SequencePlugin(){}

bool SequencePlugin::Load( CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{
	ConnectTier1Libraries( &interfaceFactory, 1 );

	// Get IMaterialSystem from MaterialSystem.dll
	void* hmMatSys = GetHandleOfModule("MaterialSystem");
	CreateInterfaceFn pfnMatSys = (CreateInterfaceFn) GetFuncAddress(hmMatSys, "CreateInterface");
	pMaterialSystem = (IMaterialSystem*) pfnMatSys("VMaterialSystem080", NULL);

	// Get ISurface from vguimatsurface.dll
	void* hmVGUIMatSurface = GetHandleOfModule("vguimatsurface");
	CreateInterfaceFn pfnVGUIMatSurface = (CreateInterfaceFn) GetFuncAddress(hmVGUIMatSurface, "CreateInterface");
	pSurface = (vgui::ISurface*) pfnVGUIMatSurface("VGUI_Surface030", NULL);

	// Get IShaderAPI from shaderapidx9.dll
	void* hmShader = GetHandleOfModule("shaderapidx9");
	CreateInterfaceFn pfnShader = (CreateInterfaceFn) GetFuncAddress(hmShader, "CreateInterface");
	pShader = (IShaderAPI*) pfnShader("ShaderApi030", NULL);

	// Get IGameUIFuncs from engine.dll
	void* hmEngine = GetHandleOfModule("engine");
	CreateInterfaceFn pfnEngine = (CreateInterfaceFn) GetFuncAddress(hmEngine, "CreateInterface");
	pGameUIFuncs = (IGameUIFuncs*) pfnEngine("VENGINE_GAMEUIFUNCS_VERSION005", NULL);

	// Register cvars & concommands
	ConVar_Register( 0 );
	return true;
}
void SequencePlugin::Unload( void )
{
	ConVar_Unregister( );
	DisconnectTier1Libraries();
}

const char *SequencePlugin::GetPluginDescription( void )
{
	return PLUGIN_DESC;
}

// Unused
void SequencePlugin::Pause( void ){}
void SequencePlugin::UnPause( void ){}
void SequencePlugin::LevelInit( char const *pMapName ){}
void SequencePlugin::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax ){}
void SequencePlugin::GameFrame( bool simulating ){}
void SequencePlugin::LevelShutdown( void ){}
void SequencePlugin::ClientActive( edict_t *pEntity ){}
void SequencePlugin::ClientDisconnect( edict_t *pEntity ){}
void SequencePlugin::ClientPutInServer( edict_t *pEntity, char const *playername ){}
void SequencePlugin::SetCommandClient( int index ){}
void SequencePlugin::ClientSettingsChanged( edict_t *pEdict ){}
PLUGIN_RESULT SequencePlugin::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen ){return PLUGIN_CONTINUE;}
PLUGIN_RESULT SequencePlugin::ClientCommand( edict_t *pEntity, const CCommand &args ){return PLUGIN_CONTINUE;}
PLUGIN_RESULT SequencePlugin::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID ){return PLUGIN_CONTINUE;}
void SequencePlugin::OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue ){}
void SequencePlugin::OnEdictAllocated( edict_t *edict ){}
void SequencePlugin::OnEdictFreed( const edict_t *edict ){}

static void ConvertModeStruct( ShaderDeviceInfo_t *pMode, const MaterialSystem_Config_t &config ) 
{
	pMode->m_DisplayMode.m_nWidth = config.m_VideoMode.m_Width;					
	pMode->m_DisplayMode.m_nHeight = config.m_VideoMode.m_Height;
	pMode->m_DisplayMode.m_Format = config.m_VideoMode.m_Format;			
	pMode->m_DisplayMode.m_nRefreshRateNumerator = config.m_VideoMode.m_RefreshRate;	
	pMode->m_DisplayMode.m_nRefreshRateDenominator = config.m_VideoMode.m_RefreshRate ? 1 : 0;	
	pMode->m_nBackBufferCount = 1;			
	pMode->m_nAASamples = config.m_nAASamples;
	pMode->m_nAAQuality = config.m_nAAQuality;
	pMode->m_nDXLevel = config.dxSupportLevel;					
	pMode->m_nWindowedSizeLimitWidth = (int)config.m_WindowedSizeLimitWidth;	
	pMode->m_nWindowedSizeLimitHeight = (int)config.m_WindowedSizeLimitHeight;

	pMode->m_bWindowed = config.Windowed();
	pMode->m_bResizing = config.Resizing();			
	pMode->m_bUseStencil = config.Stencil();
	pMode->m_bLimitWindowedSize = config.LimitWindowedSize();	
	pMode->m_bWaitForVSync = config.WaitForVSync();	
	pMode->m_bScaleToOutputResolution = config.ScaleToOutputResolution();
	pMode->m_bUsingMultipleWindows = config.UsingMultipleWindows();
}

static void sr_forceres( const CCommand& args )
{
	if (args.ArgC() < 3) {
		Warning("Usage: sr_forceres [width] [height]\n");
		return;
	}

	int w = atoi(args.Arg(1));
	int h = atoi(args.Arg(2));
	
	vmode_t *plist = NULL;
	int iCount = 0;
	pGameUIFuncs->GetVideoModes(&plist, &iCount);
	plist[0].width = w;
	plist[0].height = h;

	pSurface->ForceScreenSizeOverride(true, w, h);
	
	const MaterialSystem_Config_t &currentConfig = pMaterialSystem->GetCurrentConfigForVideoCard();

	MaterialSystem_Config_t config = currentConfig;

	config.m_VideoMode.m_Width = w;
	config.m_VideoMode.m_Height = h;
	config.SetFlag(MATSYS_VIDCFG_FLAGS_LIMIT_WINDOWED_SIZE, false);

	ShaderDeviceInfo_t deviceInfo;
	ConvertModeStruct(&deviceInfo, config);
	pShader->ChangeVideoMode(deviceInfo);

	pMaterialSystem->OverrideConfig(config, true);
}