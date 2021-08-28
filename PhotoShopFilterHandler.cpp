#include "stdafx.h"
#include "painter.h"
#include "PhotoShopFilterHandler.h"

#include <String/Convert.h>

#include <debug/debugclient.h>



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



CPhotoShopFilterHandler   PhotoShopFilterHandler;

#define userCanceledErr (-128)
#define errReportString						-30904
#define readErr			(-19)
#define writErr			(-20)
#define openErr			(-23)
#define dskFulErr			(-34)
#define ioErr				(-36)
#define eofErr				(-39)		// Also - end of descriptor error.
#define fnfErr				(-43)
#define vLckdErr			(-46)
#define fLckdErr			(-45)
#define paramErr			(-50)
#define memFullErr		(-108)
#define nilHandleErr		(-109)
#define memWZErr			(-111)

#define filterSelectorAbout 	 0
#define filterSelectorParameters 1
#define filterSelectorPrepare	 2
#define filterSelectorStart 	 3
#define filterSelectorContinue	 4
#define filterSelectorFinish	 5

#define filterBadMode		-30101	/* "the filter module does not support <mode> images" */
#define errPlugInPropertyUndefined			-30901
#define errHostDoesNotSupportColStep		-30902
#define errInvalidSamplePoint				-30903

// Color Service Selectors:
#define plugIncolorServicesChooseColor		0
#define plugIncolorServicesConvertColor		1		
#define plugIncolorServicesSamplePoint		2
#define plugIncolorServicesGetSpecialColor	3

//-------------------------------------------------------------------------------
//	Defines -- Image modes.
//-------------------------------------------------------------------------------
#define plugInModeBitmap	   0
#define plugInModeGrayScale    1
#define plugInModeIndexedColor 2
#define plugInModeRGBColor	   3
#define plugInModeCMYKColor    4
#define plugInModeHSLColor	   5
#define plugInModeHSBColor	   6
#define plugInModeMultichannel 7
#define plugInModeDuotone	   8
#define plugInModeLabColor	   9
#define plugInModeGray16	  10
#define plugInModeRGB48		  11
#define plugInModeLab48		  12
#define plugInModeCMYK64	  13
#define plugInModeDeepMultichannel 14
#define plugInModeDuotone16   15


#define noErr 0
#define errPlugInHostInsufficient			-30900
#define MACPASCAL

typedef long SPErr;

typedef unsigned long PIType;
typedef signed long  int32;
typedef signed short int16;
typedef signed char  int8;

typedef short OSErr;
typedef DWORD OSType;
typedef unsigned long ResType;

typedef unsigned long  unsigned32;
typedef unsigned short unsigned16;
typedef unsigned char  unsigned8;

typedef unsigned long	uint32;
typedef unsigned short	uint16;
typedef unsigned char	uint8;

typedef unsigned long  unsigned32;
typedef unsigned short unsigned16;
typedef unsigned char  unsigned8;

typedef long Fixed;

typedef BYTE  Boolean;

typedef LPSTR Ptr;
typedef LPSTR *Handle;

typedef unsigned char Str255[256];  /*  first byte length of string. The string is zero terminated. */
typedef const unsigned char *ConstStr255Param;

typedef struct _PIChannelPortDesc *PIChannelPort;

typedef PIChannelPort ChannelReadPort;
typedef PIChannelPort ChannelWritePort;

typedef struct PSBuffer *BufferID;

struct RGBColor
{
  WORD red;        /* Magnitude of red   component, 16 bit significant.*/
  WORD green;      /* Magnitude of green component, 16 bit significant.*/
  WORD blue;       /* Magnitude of blue  component, 16 bit significant.*/
};


//-------------------------------------------------------------------------------
//	Structures -- PiPL entry definition.
//-------------------------------------------------------------------------------
struct PIProperty
{
	// Vendor specific identifier:	
	PIType vendorID;
	
	// Identifcation key for this property type:
	PIType propertyKey;
	
	// Index within this property type. Must be unique for properties of
	// a given type in a PiPL:
	int32  propertyID;
	
	// Length of following data array. Will be rounded to multiple of 4:
	int32  propertyLength;
	
	// propertyLength bytes long. Must be multiple of 4:
	char   propertyData [1];

};

struct PIPropertyList
{
	// Properties data structure version number:
	int32 version;

	// Number of properties contained:
	int32 count;

	// The actual properties:
	PIProperty properties[1];

};

struct Point
{
	short v;
	short h;
};

struct VRect
{
	int32 top;
	int32 left;
	int32 bottom;
	int32 right;
};
	
struct VPoint
{
	int32 v;
	int32 h;
};

struct PlugInMonitor
{
	Fixed gamma;
	Fixed redX;
	Fixed redY;
	Fixed greenX;
	Fixed greenY;
	Fixed blueX;
	Fixed blueY;
	Fixed whiteX;
	Fixed whiteY;
	Fixed ambient;

};

struct PSPixelMask
{
	struct PSPixelMask *next;
	void *maskData;
	int32 rowBytes;
	int32 colBytes;
	int32 maskDescription;

};

struct PSPixelOverlay
{
	struct PSPixelOverlay *next;
	void *data;
	int32 rowBytes;
	int32 colBytes;
	unsigned8 r, g, b, opacity;
	int32 overlayAlgorithm;	// 0 for alpha overlay. 1 for inverted alpha overlay.
};

typedef struct PSPixelMap
{
	int32 version;
	VRect bounds;
	int32 imageMode;
	int32 rowBytes;
	int32 colBytes;
	int32 planeBytes;
	void *baseAddr;
	
	//---------------------------------------------------------------------------
	// Fields new in version 1:
	//---------------------------------------------------------------------------	
	PSPixelMask *mat;
	PSPixelMask *masks;
	
	// Use to set the phase of the checkerboard:
	int32 maskPhaseRow;
	int32 maskPhaseCol;

	//---------------------------------------------------------------------------
	// Fields new in version 2:
	//---------------------------------------------------------------------------	
	PSPixelOverlay *pixelOverlays;
	unsigned32 colorManagementOptions;

} PSPixelMap;

//-------------------------------------------------------------------------------
//	Structures -- ColorServicesInfo.
//-------------------------------------------------------------------------------
struct ColorServicesInfo
{
	int32 infoSize;
	int16 selector;
	int16 sourceSpace;
	int16 resultSpace;
	Boolean resultGamutInfoValid;
	Boolean resultInGamut;
	
	// Both voids must be NULL or will return paramErr:
	void *reservedSourceSpaceInfo;
	void *reservedResultSpaceInfo;

	int16 colorComponents[4];

	// Reserved must be NULL or will return paramErr:
	void *reserved;

	union
	{
		Str255 *pickerPrompt;
		Point *globalSamplePoint;
		int32 specialColorID;
	} selectorParameter;

};

struct PSImagePlane
{
	void *data;
	Rect bounds;
	int32 rowBytes;
	int32 colBytes;

};

struct PSImageMultiPlane
{
	void *data;
	Rect bounds;
	int32 rowBytes;
	int32 colBytes;
	int16 activeChannels;

};

struct PSScaling
{
	VRect sourceRect;
	VRect destinationRect;

};

struct PixelMemoryDesc
{
	void *data;
	int32 rowBits;		// Note bits not bytes!
	int32 colBits;		// Note bits not bytes!
	int32 bitOffset;
	int32 depth;

};

typedef unsigned char FilterColor [4];

#ifdef __cplusplus

extern "C"
{
#endif

	// Direct callbacks:
	typedef MACPASCAL Boolean (*TestAbortProc) (void);

	typedef MACPASCAL void (*ProgressProc) (int32 done, int32 total);

	typedef MACPASCAL void (*HostProc) (int16 selector, int32 *data);

	typedef MACPASCAL int32 (*SpaceProc) (void);

	// Handle procs:
	typedef MACPASCAL Handle (*NewPIHandleProc) (int32 size);
	typedef MACPASCAL void (*DisposePIHandleProc) (Handle h);
	typedef MACPASCAL void (*DisposeRegularHandlePIHandleProc) (Handle h);

	typedef MACPASCAL int32 (*GetPIHandleSizeProc) (Handle h);
	typedef MACPASCAL OSErr (*SetPIHandleSizeProc) (Handle h, int32 newSize);
	typedef MACPASCAL Ptr (*LockPIHandleProc) (Handle h, Boolean moveHigh);
	typedef MACPASCAL void (*UnlockPIHandleProc) (Handle h);

	typedef MACPASCAL void (*RecoverSpaceProc) (int32 size);

	typedef MACPASCAL OSErr (*AllocateBufferProc) (int32 size, BufferID *bufferID);
	typedef MACPASCAL Ptr (*LockBufferProc) (BufferID bufferID, Boolean moveHigh);
	typedef MACPASCAL void (*UnlockBufferProc) (BufferID bufferID);
	typedef MACPASCAL void (*FreeBufferProc) (BufferID bufferID);
	typedef MACPASCAL int32 (*BufferSpaceProc) (void);

	typedef MACPASCAL int16 (*CountPIResourcesProc) (ResType type);
	typedef MACPASCAL Handle (*GetPIResourceProc) (ResType type, int16 index);
	typedef MACPASCAL void (*DeletePIResourceProc) (ResType type, int16 index);
	typedef MACPASCAL OSErr (*AddPIResourceProc) (ResType type, Handle data);

	typedef MACPASCAL void (*ProcessEventProc) (void *event);

	typedef MACPASCAL OSErr (*DisplayPixelsProc) (const PSPixelMap *source,
												  const VRect *srcRect,
												  int32 dstRow,
												  int32 dstCol,
												  void *platformContext);
											
	typedef MACPASCAL OSErr (*AdvanceStateProc) (void);

	// Property suite:
	typedef MACPASCAL OSErr (*GetPropertyProc) (PIType signature,
												PIType key,
												int32 index,
												int32 *simpleProperty,
												Handle *complexProperty);
	typedef MACPASCAL OSErr (*SetPropertyProc) (PIType signature,
												PIType key,
												int32 index,
												int32 simpleProperty,
												Handle complexProperty);

	typedef MACPASCAL OSErr (*ColorServicesProc) (ColorServicesInfo *info);

	typedef MACPASCAL OSErr (*PIResampleProc) (PSImagePlane *source,
											   PSImagePlane *destination,
											   Rect *area,
											   Fixed *coords,
											   int16 method);

	typedef MACPASCAL OSErr (*PIResampleMultiProc) (PSImageMultiPlane *source,
												   PSImageMultiPlane *destination,
												   Rect *area,
												   Fixed *coords,
												   int16 method);
											   
	// Channel ports suite:
	typedef MACPASCAL OSErr (*ReadPixelsProc) (ChannelReadPort port,
											   const PSScaling *scaling,
											   const VRect *writeRect,
											   		/* Rectangle in destination coordinates. */
											   const PixelMemoryDesc *destination,
											   VRect *wroteRect);
	typedef MACPASCAL OSErr (*WriteBasePixelsProc) (ChannelWritePort port,
													const VRect *writeRect,
													const PixelMemoryDesc *source);
	typedef MACPASCAL OSErr (*ReadPortForWritePortProc) (ChannelReadPort *readPort,
														 ChannelWritePort writePort);
	
#ifdef __cplusplus
}
#endif

#pragma pack(push,4)

struct BufferProcs
{

	int16 bufferProcsVersion;
	
	int16 numBufferProcs;

	AllocateBufferProc allocateProc;

	LockBufferProc lockProc;

	UnlockBufferProc unlockProc;

	FreeBufferProc freeProc;
	
	BufferSpaceProc spaceProc;

};

struct ResourceProcs
{
	
	int16 resourceProcsVersion;
	
	int16 numResourceProcs;
	
	CountPIResourcesProc countProc;
	
	GetPIResourceProc getProc;
	
	DeletePIResourceProc deleteProc;
	
	AddPIResourceProc addProc;
	
};

struct HandleProcs
{
	
	int16 handleProcsVersion;
	
	int16 numHandleProcs;
	
	NewPIHandleProc newProc;
	
	DisposePIHandleProc disposeProc;

	GetPIHandleSizeProc getSizeProc;
	
	SetPIHandleSizeProc setSizeProc;
	
	LockPIHandleProc lockProc;
	
	UnlockPIHandleProc unlockProc;
	
	RecoverSpaceProc recoverSpaceProc;
	
	DisposeRegularHandlePIHandleProc disposeRegularHandleProc;
	
};

struct ImageServicesProcs
{
	
	int16 imageServicesProcsVersion;
	int16 numImageServicesProcs;
	
	PIResampleProc interpolate1DProc;
	PIResampleProc interpolate2DProc;
	PIResampleMultiProc interpolate1DMultiProc;
	PIResampleMultiProc interpolate2DMultiProc;
	
};

struct PropertyProcs
{
	
	int16 propertyProcsVersion;
	int16 numPropertyProcs;
	
	GetPropertyProc	getPropertyProc;
	SetPropertyProc setPropertyProc;
	
};

typedef Handle PIDescriptorHandle;

typedef struct PIOpaqueWriteDescriptor*	PIWriteDescriptor;
typedef struct PIOpaqueReadDescriptor*	PIReadDescriptor;

typedef unsigned long	DescriptorEventID;
typedef unsigned long	DescriptorClassID;

typedef unsigned long	DescriptorKeyID;
typedef unsigned long	DescriptorTypeID;
typedef unsigned long	DescriptorUnitID;
typedef unsigned long	DescriptorEnumID;
typedef unsigned long	DescriptorEnumTypeID;

typedef unsigned long	DescriptorFormID;

typedef DescriptorKeyID	DescriptorKeyIDArray[];

struct PIDescriptorSimpleReference
{
	DescriptorTypeID	desiredClass;
	DescriptorKeyID		keyForm;
	struct _keyData
		{
		Str255				name;
		int32				index;
		DescriptorTypeID	type;
		DescriptorEnumID	value;
		}				keyData;
};

#ifdef __cplusplus
extern "C"
	{
#endif

//-------------------------------------------------------------------------------
//	WriteDescriptor callbacks.
//-------------------------------------------------------------------------------
typedef MACPASCAL PIWriteDescriptor (*OpenWriteDescriptorProc)(void);
typedef MACPASCAL OSErr (*CloseWriteDescriptorProc)(PIWriteDescriptor, PIDescriptorHandle*);

typedef MACPASCAL OSErr (*PutIntegerProc)(PIWriteDescriptor, DescriptorKeyID, int32);
typedef MACPASCAL OSErr (*PutFloatProc)(PIWriteDescriptor, DescriptorKeyID, const double*);
typedef MACPASCAL OSErr (*PutUnitFloatProc)(PIWriteDescriptor, DescriptorKeyID, DescriptorUnitID, const double*);
typedef MACPASCAL OSErr (*PutBooleanProc)(PIWriteDescriptor, DescriptorKeyID, Boolean);
typedef MACPASCAL OSErr (*PutTextProc)(PIWriteDescriptor, DescriptorKeyID, Handle);
typedef MACPASCAL OSErr (*PutAliasProc)(PIWriteDescriptor, DescriptorKeyID, Handle);

typedef MACPASCAL OSErr (*PutEnumeratedProc)(PIWriteDescriptor, DescriptorKeyID key, DescriptorTypeID type, DescriptorEnumID value);
typedef MACPASCAL OSErr (*PutClassProc)(PIWriteDescriptor, DescriptorKeyID, DescriptorTypeID);
typedef MACPASCAL OSErr (*PutSimpleReferenceProc)(PIWriteDescriptor, DescriptorKeyID, const PIDescriptorSimpleReference*);
typedef MACPASCAL OSErr (*PutObjectProc)(PIWriteDescriptor, DescriptorKeyID, DescriptorTypeID, PIDescriptorHandle);

typedef MACPASCAL OSErr	(*PutCountProc)(PIWriteDescriptor, DescriptorKeyID, uint32 count);

typedef MACPASCAL OSErr (*PutStringProc)(PIWriteDescriptor, DescriptorKeyID, ConstStr255Param);

// Scoped classes are not for use by plug-ins in Photoshop 4.0:
typedef MACPASCAL OSErr (*PutScopedClassProc)(PIWriteDescriptor, DescriptorKeyID, DescriptorTypeID);
typedef MACPASCAL OSErr (*PutScopedObjectProc)(PIWriteDescriptor, DescriptorKeyID, DescriptorTypeID, PIDescriptorHandle);

//-------------------------------------------------------------------------------
//	ReadDescriptor callbacks.
//-------------------------------------------------------------------------------
typedef MACPASCAL PIReadDescriptor (*OpenReadDescriptorProc)(PIDescriptorHandle, DescriptorKeyIDArray);
typedef MACPASCAL OSErr (*CloseReadDescriptorProc)(PIReadDescriptor);
typedef MACPASCAL Boolean (*GetKeyProc)(PIReadDescriptor, DescriptorKeyID* key, DescriptorTypeID* type, int32* flags);

typedef MACPASCAL OSErr (*GetIntegerProc)(PIReadDescriptor, int32*);
typedef MACPASCAL OSErr (*GetFloatProc)(PIReadDescriptor, double*);
typedef MACPASCAL OSErr (*GetUnitFloatProc)(PIReadDescriptor, DescriptorUnitID*, double*);
typedef MACPASCAL OSErr (*GetBooleanProc)(PIReadDescriptor, Boolean*);
typedef MACPASCAL OSErr (*GetTextProc)(PIReadDescriptor, Handle*);
typedef MACPASCAL OSErr (*GetAliasProc)(PIReadDescriptor, Handle*);

typedef MACPASCAL OSErr (*GetEnumeratedProc)(PIReadDescriptor, DescriptorEnumID*);
typedef MACPASCAL OSErr (*GetClassProc)(PIReadDescriptor, DescriptorTypeID*);
typedef MACPASCAL OSErr (*GetSimpleReferenceProc)(PIReadDescriptor, PIDescriptorSimpleReference*);
typedef MACPASCAL OSErr (*GetObjectProc)(PIReadDescriptor, DescriptorTypeID*, PIDescriptorHandle*);

typedef MACPASCAL OSErr (*GetCountProc)(PIReadDescriptor, uint32*);

typedef MACPASCAL OSErr (*GetStringProc)(PIReadDescriptor, Str255*);
typedef MACPASCAL OSErr (*GetPinnedIntegerProc)(PIReadDescriptor, int32 min, int32 max, int32*);
typedef MACPASCAL OSErr (*GetPinnedFloatProc)(PIReadDescriptor, const double* min, const double* max, double*);
typedef MACPASCAL OSErr (*GetPinnedUnitFloatProc)(PIReadDescriptor, const double* min, const double* max, DescriptorUnitID*, double*);

//-------------------------------------------------------------------------------

#ifdef __cplusplus
	}
#endif


struct WriteDescriptorProcs
{
	int16 writeDescriptorProcsVersion;
	int16 numWriteDescriptorProcs;
	
	OpenWriteDescriptorProc		openWriteDescriptorProc;
	CloseWriteDescriptorProc	closeWriteDescriptorProc;

	PutIntegerProc				putIntegerProc;
	PutFloatProc				putFloatProc;
	PutUnitFloatProc			putUnitFloatProc;
	PutBooleanProc				putBooleanProc;
	PutTextProc					putTextProc;
	PutAliasProc				putAliasProc;

	PutEnumeratedProc			putEnumeratedProc;
	PutClassProc				putClassProc;
	PutSimpleReferenceProc		putSimpleReferenceProc;
	PutObjectProc				putObjectProc;
	
	PutCountProc				putCountProc;
	
	PutStringProc				putStringProc;
	
	/* Scoped classes are not for use by plug-ins in Photoshop 4.0 */
	
	PutScopedClassProc			putScopedClassProc;
	PutScopedObjectProc			putScopedObjectProc;
	
};


struct ReadDescriptorProcs
{
	int16 readDescriptorProcsVersion;
	int16 numReadDescriptorProcs;
	
	OpenReadDescriptorProc		openReadDescriptorProc;
	CloseReadDescriptorProc		closeReadDescriptorProc;
	GetKeyProc					getKeyProc;

	GetIntegerProc				getIntegerProc;
	GetFloatProc				getFloatProc;
	GetUnitFloatProc			getUnitFloatProc;
	GetBooleanProc				getBooleanProc;
	GetTextProc					getTextProc;
	GetAliasProc				getAliasProc;

	GetEnumeratedProc			getEnumeratedProc;
	GetClassProc				getClassProc;
	GetSimpleReferenceProc		getSimpleReferenceProc;
	GetObjectProc				getObjectProc;

	GetCountProc				getCountProc;
	
	GetStringProc				getStringProc;
	GetPinnedIntegerProc		getPinnedIntegerProc;
	GetPinnedFloatProc			getPinnedFloatProc;
	GetPinnedUnitFloatProc		getPinnedUnitFloatProc;

};

struct PIDescriptorParameters
{
	int16					descriptorParametersVersion;
	int16					playInfo;
	int16					recordInfo;
	
	PIDescriptorHandle		descriptor;
	
	WriteDescriptorProcs*	writeDescriptorProcs;
	ReadDescriptorProcs*	readDescriptorProcs;
};

struct ChannelPortProcs
{
	
	int16 channelPortProcsVersion;
	int16 numChannelPortProcs;
	
	ReadPixelsProc readPixelsProc;
	WriteBasePixelsProc writeBasePixelsProc;
	ReadPortForWritePortProc readPortForWritePortProc;
	
};

typedef unsigned8 PILookUpTable [256];

typedef PILookUpTable LookUpTable;

struct ReadChannelDesc
{
	
	// The minimum and maximum version which can be used to
	// interpret this record:
	int32 minVersion;
	int32 maxVersion;

	//---------------------------------------------------------------------------
	//	Version 0 fields:
	//---------------------------------------------------------------------------
		
	// The next descriptor in the list:
	struct ReadChannelDesc *next;
	
	PIChannelPort port;		// Port to use for reading.
	
	VRect bounds;			// Bounds of the channel data.
	int32 depth;			// Depth of the data.
	
	VPoint tileSize;		// Size of the tiles.
	VPoint tileOrigin;		// Origin for the tiles.
	
	Boolean target;			// Is this a target channel.
	Boolean shown;			// Is this channel shown?
	
	int16 channelType;		// Channel type. See above.
	
	void *contextInfo;		// Pointer to additional info dependent on context.
	
	const char *name;		// Name of the channel as a C string.

	//---------------------------------------------------------------------------
	//	Version 1 fields:
	//---------------------------------------------------------------------------

	PIChannelPort writePort;	/* Port to use for writing if writing is allowed.
								 This may not point back to the same collection
								 of pixels. In fact, it generally, won't. */
	
	//---------------------------------------------------------------------------
	//	Version 2 fields:
	//---------------------------------------------------------------------------

	unsigned32 alphaID;		/* Persistent ID for the channel. Only set for alpha
							   channels. */
	
};

struct ReadImageDocumentDesc
{
	
	// The minimum and maximum version which can be used to
	// interpret this record:
	int32 minVersion;
	int32 maxVersion;
	
	//---------------------------------------------------------------------------
	//	Version 0 fields:
	//---------------------------------------------------------------------------
	
	int32 imageMode;		// Color mode for the document.
	int32 depth;			// Default depth for the document.
	VRect bounds;			// Bounds for the document.
	
	Fixed hResolution;		// Resolution for the document.
	Fixed vResolution;
	
	LookUpTable redLUT;		// Color table for indexed color and duotone.
	LookUpTable greenLUT;
	LookUpTable blueLUT;
	
	// Composite channels in the target layer:
	ReadChannelDesc *targetCompositeChannels;
							
	// Transparency channel for the target layer:
	ReadChannelDesc *targetTransparency;
							
	// Layer mask for the target layer:
	ReadChannelDesc *targetLayerMask;
							
	// Composite channels in the merged data:
	ReadChannelDesc *mergedCompositeChannels;
							
	// Transparency channel for the merged data:
	ReadChannelDesc *mergedTransparency;
							
	// Alpha channels:
	ReadChannelDesc *alphaChannels;
							
	// Selection mask, if any:
	ReadChannelDesc *selection;

	// New with version 1.

	// The file specification if any.

	struct SPPlatformFileSpecification_t *fileSpec;
	
};

#define SPAPI

typedef int SPBoolean;

struct SPBasicSuite 
{

	SPAPI SPErr (*AcquireSuite)( char *name, long version, void **suite );
	SPAPI SPErr (*ReleaseSuite)( char *name, long version );
	SPAPI SPBoolean (*IsEqual)( char *token1, char *token2 );
	SPAPI SPErr (*AllocateBlock)( long size, void **block );
	SPAPI SPErr (*FreeBlock)( void *block );
	SPAPI SPErr (*ReallocateBlock)( void *block, long newSize, void **newblock );
	SPAPI SPErr (*Undefined)( void );
};

/********************************************************************************/

struct FilterRecord 
{

	int32		serialNumber;		/* Host's serial number, to allow
									   copy protected plug-in modules. */

	TestAbortProc	abortProc;		/* The plug-in module may call this no-argument
									   BOOLEAN function (using Pascal calling
									   conventions) several times a second during long
									   operations to allow the user to abort the operation.
									   If it returns TRUE, the operation should be aborted
									   (and a positive error code returned). */

	ProgressProc	progressProc;	/* The plug-in module may call this two-argument
									   procedure periodically to update a progress
									   indicator.  The first parameter is the number
									   of operations completed; the second is the total
									   number of operations. */

	Handle		parameters; 		/* A handle, initialized to NIL by Photoshop.
									   This should be used to hold the filter's
									   current parameters. */

	Point		imageSize;			/* Size of image */
	int16		planes; 			/* Samples per pixel */
	Rect		filterRect; 		/* Rectangle to filter */

	RGBColor	background; 		/* Current background color */
	RGBColor	foreground; 		/* Current foreground color */

	int32		maxSpace;			/* Maximum possible total of data and buffer space */

	int32		bufferSpace;		/* If the plug-in filter needs to allocate
									   large internal buffers, the filterSelectorPrepare
									   routine should set this field to the number
									   of bytes the filterSelectorStart routine is
									   planning to allocate.  Relocatable blocks should
									   be used if possible. */

	Rect		inRect; 			/* Requested input rectangle. Must be a subset of
									   the image's bounding rectangle. */
	int16		inLoPlane;			/* First requested input plane */
	int16		inHiPlane;			/* Last requested input plane */

	Rect		outRect;			/* Requested output rectangle. Must be a subset of
									   filterRect. */
	int16		outLoPlane; 		/* First requested output plane */
	int16		outHiPlane; 		/* Last requested output plane */

	void *		inData; 			/* Pointer to input rectangle. If more than one
									   plane was requested, the data is interleaved. */
	int32		inRowBytes; 		/* Offset between input rows */
	void *		outData;			/* Pointer to output rectangle. If more than one
									   plane was requested, the data is interleaved. */
	int32		outRowBytes;		/* Offset between output rows */

	Boolean 	isFloating; 		/* Set to true if the selection is floating */
	Boolean 	haveMask;			/* Set to true if there is a selection mask */
	Boolean 	autoMask;			/* If there is a mask, and the selection is not
									   floating, the plug-in can change this field to
									   false to turn off auto-masking. */

	Rect		maskRect;			/* Requested mask rectangle.  Must be a subset of
									   filterRect. Should only be used if haveMask is
									   true. */

	void *		maskData;			/* Pointer to (read only) mask data. */
	int32		maskRowBytes;		/* Offset between mask rows */

	FilterColor backColor;			/* Background color in native color space */
	FilterColor foreColor;			/* Foreground color in native color space */

	OSType		hostSig;			/* Creator code for host application */
	HostProc	hostProc;			/* Host specific callback procedure */

	int16		imageMode;			/* Image mode */

	Fixed		imageHRes;			/* Pixels per inch */
	Fixed		imageVRes;			/* Pixels per inch */

	Point		floatCoord; 		/* Top left coordinate of selection */
	Point		wholeSize;			/* Size of image selection is floating over */

	PlugInMonitor	monitor;		/* Information on current monitor */

	void 		*platformData;		/* Platform specific information. */

	BufferProcs *bufferProcs;		/* The host buffer procedures. */
	
	ResourceProcs *resourceProcs;	/* The host plug-in resource procedures. */
	
	ProcessEventProc processEvent;	/* Pass event to the application. */
	
	DisplayPixelsProc displayPixels;/* Display dithered pixels. */

	HandleProcs	*handleProcs;		/* Platform independent handle manipulation. */

	/* New in 3.0. */

	Boolean		supportsDummyChannels;	/* Does the host support dummy channels? */
	
	Boolean 	supportsAlternateLayouts;	/* Does the host support alternate
											   data layouts. */

	int16		wantLayout;			/* The layout to use for the data.
										See PIGeneral for the values.  */
	
	int16		filterCase;			/* Filter case. If zero, then the plug-in
									   should look at haveMask and isFloating 
									   since older hosts will not set this
									   field. */
									   
	int16		dummyPlaneValue;	/* 0..255 = fill value
									   -1 = leave undefined
									   All others generate errors. */
	
	void *		premiereHook;		/* A hook for Premiere.  See the Premiere
									   Developer's Kit. */
									   
	AdvanceStateProc advanceState;	/* Advance from start to continue or
									   continue to continue. */
									   
	Boolean		supportsAbsolute;	/* Does the host support absolute plane
									   indexing? */
									   
	Boolean		wantsAbsolute;		/* Does the plug-in want absolute plane
									   indexing? (input only) */
									   
	GetPropertyProc	getPropertyObsolete;	/* Use the suite if available */
									   
	Boolean		cannotUndo;			/* If set to TRUE, then undo will not
									   be enabled for this command. */
									   
	Boolean		supportsPadding;	/* Does the host support requests outside
									   the image area? */
	
	int16		inputPadding;		/* Instructions for padding the input. */
	
	int16		outputPadding;		/* Instructions for padding the output. */
	
	int16		maskPadding;		/* Padding instructions for the mask. */
	
	char	 	samplingSupport;	/* Does the host support sampling the
									   input and mask? */
									   
	char		reservedByte;		/* Alignment. */
	
	Fixed		inputRate;			/* Input sample rate. */
	Fixed		maskRate;			/* Mask sample rate. */

	ColorServicesProc colorServices; /* Routine to access color services. */
	
	/* Photoshop structures its data as follows for plug-ins when processing
	   layer data:
			target layer channels
			transparency mask for target layer
			layer mask channels for target layer
			inverted layer mask channels for target layer
			non-layer channels
		When processing non-layer data (including running a filter on the
		layer mask alone), Photoshop structures the data as consisting only
		of non-layer channels.  It indicates this structure through a series
		of short counts.  The transparency count must be either 0 or 1. */
		
	int16		inLayerPlanes;
	int16		inTransparencyMask;
	int16		inLayerMasks;
	int16		inInvertedLayerMasks;
	int16		inNonLayerPlanes;
			
	int16		outLayerPlanes;
	int16		outTransparencyMask;
	int16		outLayerMasks;
	int16		outInvertedLayerMasks;
	int16		outNonLayerPlanes;
			
	int16		absLayerPlanes;
	int16		absTransparencyMask;
	int16		absLayerMasks;
	int16		absInvertedLayerMasks;
	int16		absNonLayerPlanes;
	
	/* We allow for extra planes in the input and the output.  These planes
	   will be filled with dummyPlaneValue at those times when we build the
	   buffers.  These features will only be available if supportsDummyPlanes
	   is TRUE. */
	
	int16		inPreDummyPlanes;	/* Extra planes to allocate in the input. */
	int16		inPostDummyPlanes;
	
	int16		outPreDummyPlanes;	/* Extra planes to allocate in the output. */
	int16		outPostDummyPlanes;
	
	/* If the plug-in makes use of the layout options, then the following
	   fields should be obeyed for identifying the steps between components.
	   The last component in the list will always have a step of one. */
	
	int32		inColumnBytes;		/* Step between input columns. */
	int32		inPlaneBytes;		/* Step between input planes. */
			
	int32		outColumnBytes;		/* Step between output columns. */
	int32		outPlaneBytes;		/* Step between output planes. */
			
	/* New in 3.0.4. */
	
	ImageServicesProcs *imageServicesProcs;
									/* Suite of image processing callbacks. */
			
	PropertyProcs	*propertyProcs;	/*	Routines to query and set document and
										view properties. The plug-in needs to
										dispose of the handle returned for
										complex properties (the plug-in also
										maintains ownership of handles for
										set properties.
									*/
			
	int16		inTileHeight;		/* Tiling for the input. Zero if not set. */
	int16		inTileWidth;
	Point		inTileOrigin;
	
	int16		absTileHeight;		/* Tiling for the absolute data. */
	int16		absTileWidth;
	Point		absTileOrigin;
	
	int16		outTileHeight;		/* Tiling for the output. */
	int16		outTileWidth;
	Point		outTileOrigin;
	
	int16		maskTileHeight;		/* Tiling for the mask. */
	int16		maskTileWidth;
	Point		maskTileOrigin;
	
	/* New in 4.0 */
	
	PIDescriptorParameters	*descriptorParameters;	/* For recording and playback */
	Str255					*errorString;			/* For silent and errReportString */
			
	ChannelPortProcs *channelPortProcs;
									/* Suite for passing pixels through channel ports. */
			
	ReadImageDocumentDesc *documentInfo;	/* The document info for the document being filtered. */

	/* New in 5.0 */

	SPBasicSuite	*sSPBasic;		/* SuitePea basic suite */
	void			*plugInRef;		/* plugin reference used by SuitePea*/
	
	int32			depth;			/* bit depth per channel (1,8,16) */
	
	/* New in 6.0 */
		
	Handle			iCCprofileData;		/* Handle containing the ICC profile for the image. (NULL if none)
												Photoshop allocates the handle using Photoshop's handle suite
												The handle is unlocked while calling the plug-in
												The handle will be valid from Start to Finish
												Photoshop will free the handle after Finish
										*/
	
	int32			iCCprofileSize;		/* size of profile. */
	
	int32			canUseICCProfiles;	/* non-zero if the host can export ICC profiles 
											if this is zero, you'd better not set or dereference iCCprofileData :-)
										*/
	
	
	
	
	char		reserved [54]; 	/* Set to zero */

};

#pragma pack(pop)


#define long2fixed(value)	(value << 16)
#define fixed2long(value)	(value >> 16)




TestAbortProc PSAbortProc()
{
  return FALSE;
}



static void
progress_proc (long done,
              long total)
{
  //gimp_progress_update ((float) done / total);
}

static void
process_event_proc (void *event)
{
}

struct PlatformData 
{
	long hwnd;        /* window to own dialogs. */
};

static PlatformData platform;

static BufferProcs *buffer_procs = NULL;
static ChannelPortProcs *channel_port_procs = NULL;
static PIDescriptorParameters *descriptor_parameters = NULL;
static HandleProcs *handle_procs = NULL;
static ImageServicesProcs *image_services_procs = NULL;
static PropertyProcs *property_procs = NULL;
static ResourceProcs *resource_procs = NULL;
static SPBasicSuite *basic_suite = NULL;

#define filterBadParameters -30100	/* "a problem with the filter module interface" */

static OSErr 
display_pixels_proc (const PSPixelMap *source,
                     const VRect      *srcRect,
                     int32             dstRow,
                     int32             dstCol,
                     void             *platformContext)
{
  HDC hdc = (HDC) platformContext, hmemdc;
  HBITMAP hbitmap, holdbm;
  struct {
    BITMAPINFOHEADER bi;
    union {
      WORD bmiIndices[256];
      DWORD bmiMasks[3];
      RGBQUAD bmiColors[256];
    } u;
  } bmi;    
  BYTE *bits;
  int i, x, y, bpp = 0;
  BYTE *p, *q;
  int w = srcRect->right - srcRect->left, h = srcRect->bottom - srcRect->top;
  int bpl;

#ifdef PSPI_WITH_DEBUGGING
  if (debug_mask & PSPI_DEBUG_MISC_CALLBACKS)
    {
      g_print (G_STRLOC ": source->bounds:%ld:%ldx%ld:%ld\n",
	       source->bounds.left, source->bounds.right,
	       source->bounds.top, source->bounds.bottom);
      g_print ("  srcRect:%ld:%ldx%ld:%ld\n",
	       srcRect->left, srcRect->right,
	       srcRect->top, srcRect->bottom);
      g_print ("  imageMode:%s", image_mode_string (source->imageMode));
      g_print ("  rowBytes:%ld colBytes:%ld planeBytes:%ld baseAddr:%p\n",
	       source->rowBytes, source->colBytes,
	       source->planeBytes,source->baseAddr);
      g_print ("  hdc:%p\n", hdc);
    }
#endif /* PSPI_WITH_DEBUGGING */

  /* Some plug-ins call displayPixels with bogus parameters */
  if (hdc == NULL || source->rowBytes == 0 || source->baseAddr == 0)
    return filterBadParameters;

  bmi.bi.biSize = sizeof (BITMAPINFOHEADER);
  bmi.bi.biWidth = w;
  bmi.bi.biHeight = -h;
  bmi.bi.biPlanes = 1;
  switch (source->imageMode)
    {
    case plugInModeGrayScale:
      bpp = 1;
      bmi.bi.biBitCount = 8;
      for (i = 0; i < 255; i++)
	{
	  bmi.u.bmiColors[i].rgbBlue =
	    bmi.u.bmiColors[i].rgbGreen =
	    bmi.u.bmiColors[i].rgbRed = i;
	  bmi.u.bmiColors[i].rgbReserved = 0;
	}
      break;
    case plugInModeRGBColor:
      bpp = 3;
      bmi.bi.biBitCount = 24;
      break;
    default:
      //g_assert_not_reached ();
      break;
    }
  bmi.bi.biCompression = BI_RGB;
  bmi.bi.biSizeImage = 0;
  bmi.bi.biXPelsPerMeter = bmi.bi.biYPelsPerMeter = 0;
  bmi.bi.biClrUsed = 0;
  bmi.bi.biClrImportant = 0;

  if ((hbitmap = CreateDIBSection (hdc, (BITMAPINFO *) &bmi, DIB_RGB_COLORS,
				   (void **) &bits, NULL, 0)) == NULL)
    {
    /*
      g_message (_("pspi: CreateDIBSection() failed: %s"),
		 g_win32_error_message (GetLastError ()));
     */
      return noErr;
    }

  bpl = ((w*bpp - 1)/4 + 1) * 4;
  if (bpp == 1)
    for (y = 0; y < h; y++)
      {
	memmove (bits + bpl*y,
		 ((char *) source->baseAddr) + srcRect->left + source->rowBytes*y,
		 w);
      }
  else
    for (y = 0; y < h; y++)
      {
	p = bits + bpl*y;
	q = ((BYTE *) source->baseAddr) + srcRect->left*source->colBytes +
	  source->rowBytes*y;
	for (x = 0; x < w; x++)
	  {
	    /* dest DIB is BGR, source is RGB */
	    p[0] = q[2];
	    p[1] = q[1];
	    p[2] = q[0];
	    p += 3;
	    q += source->colBytes;
	  }
      }
  
  hmemdc = CreateCompatibleDC (hdc);
  holdbm = (HBITMAP)SelectObject (hmemdc, hbitmap);

  if (!BitBlt (hdc, dstCol, dstRow, bmi.bi.biWidth, -bmi.bi.biHeight, hmemdc,
	       0, 0, SRCCOPY))
  {
  /*
    g_message (_("pspi: BitBlt() failed: %s"),
	       g_win32_error_message (GetLastError ()));
         */
  }

  SelectObject (hmemdc, holdbm);
  DeleteDC (hmemdc);
  DeleteObject (hbitmap);

  return noErr;
}


/*-PSStoreResult--------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CPhotoShopFilterHandler::PSStoreResult( FilterRecord& filter, GR::Graphic::Image *pImage )
{

  if ( !filter.outData )
  {
    return;
  }

  // Rect erzeugen (Breite*Höhe*3 (RGB))
  int   iWidth = ( outRect.right - outRect.left + 1 ),
        iHeight = ( outRect.bottom - outRect.top + 1 );

  /*
  if ( ( iWidth == 0 )
  ||   ( iHeight == 0 ) )
  {
    return;
  }
  if ( ( filter.outRect.top == 0 )
  &&   ( filter.outRect.left == 0 )
  &&   ( filter.outRect.bottom == 0 )
  &&   ( filter.outRect.right == 0 ) )
  {
    return;
  }
  */

  dh::Log( "storeresult %d,%d - %d,%d", 
           outRect.left,
           outRect.top,
           outRect.right,
           outRect.bottom );


  CMaskedContextDescriptor *pCD = new CMaskedContextDescriptor( pImage, NULL );

  BYTE  *pData = (BYTE*)filter.outData,
        *pRowData = pData;
  DWORD   dwColor;

  for ( int i = 0; i < iHeight; i++ )
  {
    pData = pRowData;
    for ( int j = 0; j < iWidth; j++ )
    {
      dwColor =   ( pData[0] << 16 )
                + ( pData[1] << 8 )
                + ( pData[2] );
      pCD->PutPixel( outRect.left + j, outRect.top + i, dwColor );
      pData += 3;
    }
    pRowData += outStride;// * outPlanes;
    //pData += ( outStride - iWidth * 3 );
  }
  delete pCD;

}

/*-PSPrepareFilterRect--------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CPhotoShopFilterHandler::PSPrepareFilterRect( FilterRecord& filter, GR::Graphic::Image *pImage, GR::Graphic::Image *pOut )
{

  if ( filter.inData )
  {
    delete[] filter.inData;
    filter.inData = NULL;
  }
  if ( filter.outData )
  {
    delete[] filter.outData;
    filter.outData = NULL;
  }

  /*
  dh::Log( "request inplane %d - %d", filter.inLoPlane, filter.inHiPlane );
  dh::Log( "request outplane %d - %d", filter.outLoPlane, filter.outHiPlane );
  //dh::Log( "request inlayerplanes %d", filter.inLayerPlanes );
  //dh::Log( "request outlayerplanes %d", filter.outLayerPlanes );
  */

  // Rect erzeugen (Breite*Höhe*3 (RGB))
  int   iWidth = ( filter.inRect.right - filter.inRect.left + 1 ),
        iHeight = ( filter.inRect.bottom - filter.inRect.top + 1 );

  // neu - Planes
  filter.inData = new BYTE[iWidth * iHeight * ( filter.inHiPlane - filter.inLoPlane + 1 )];// * 3];

  /*
  if (nplanes == pr->bpp && gimpw == w)
	{
	  gimp_pixel_rgn_get_rect (pr, *buf, rect->left, rect->top,
				   gimpw, gimph);
	}
      else
	{
	  guchar *row = g_malloc (pr->bpp * gimpw);
	  gint i, j, y;

	  for (y = rect->top; y < rect->top + gimph; y++)
	    {
	      gimp_pixel_rgn_get_row (pr, row, rect->left, y, gimpw);
	      for (i = loplane; i <= hiplane; i++)
		{
		  guchar *p = row + i;
		  guchar *q = *buf + (y - rect->top) * *stride + (i - loplane);
		  for (j = 0; j < gimpw; j++)
		    {
		      *q = *p;
		      p += pr->bpp;
		      q += nplanes;
		    }
		}
	    }
	  g_free (row);
	}
  */

  if ( ( filter.outRect.left != 0 )
  ||   ( filter.outRect.top != 0 )
  ||   ( filter.outRect.right != 0 )
  ||   ( filter.outRect.bottom != 0 ) )
  {
    //filter.outData = new BYTE[( filter.outRect.right - filter.outRect.left + 1 ) * ( filter.outRect.bottom - filter.outRect.top + 1 ) * 3];

    //filter.outData = new BYTE[( filter.outRect.right - filter.outRect.left + 1 ) * ( filter.outRect.bottom - filter.outRect.top + 1 ) * ( filter.outHiPlane - filter.outLoPlane + 1 ) * 3];
    filter.outData = new BYTE[( filter.outRect.right - filter.outRect.left + 1 ) * ( filter.outRect.bottom - filter.outRect.top + 1 ) * ( filter.outHiPlane - filter.outLoPlane + 1 )];
    
    outRect.left = filter.outRect.left;
    outRect.top = filter.outRect.top;
    outRect.right = filter.outRect.right;
    outRect.bottom = filter.outRect.bottom;
    //filter.outRowBytes = ( filter.outRect.right - filter.outRect.left + 1 ) * 3;
    //filter.outRowBytes = ( filter.outRect.right - filter.outRect.left + 1 ) * ( filter.outHiPlane - filter.outLoPlane + 1 ) * 3;
    //outStride = ( filter.outRect.right - filter.outRect.left + 1 ) * ( filter.outHiPlane - filter.outLoPlane + 1 ) * 3;
    outStride = ( filter.outRect.right - filter.outRect.left + 1 ) * ( filter.outHiPlane - filter.outLoPlane + 1 );
    //outStride = ( filter.outRect.right - filter.outRect.left + 1 ) * 3;

    outPlanes = ( filter.outHiPlane - filter.outLoPlane + 1 ); 


    BYTE  *pData = (BYTE*)filter.outData;
    DWORD   dwColor;

    CMaskedContextDescriptor *pCD = new CMaskedContextDescriptor( pOut, NULL );
    for ( int i = filter.outRect.top; i <= filter.outRect.bottom; i++ )
    {
      for ( int j = filter.outRect.left; j <= filter.outRect.right; ++j )
      {
        dwColor = pCD->GetPixel( j, i );

        *pData++ = (BYTE)( ( dwColor & 0xff0000 ) >> 16 );
        *pData++ = (BYTE)( ( dwColor & 0xff00 ) >> 8 );
        *pData++ = (BYTE)( ( dwColor & 0xff ) );
      }
    }
    delete pCD;

  }

  CMaskedContextDescriptor *pCD = new CMaskedContextDescriptor( pImage, NULL );

  int   iInputRate = fixed2long( filter.inputRate );

  if ( iInputRate <= 0 )
  {
    iInputRate = 1;
  }


  BYTE  *pData = (BYTE*)filter.inData;
  DWORD   dwColor;

  for ( int i = filter.inRect.top * iInputRate; i <= filter.inRect.bottom * iInputRate; i += iInputRate )
  {
    for ( int j = filter.inRect.left * iInputRate; j <= filter.inRect.right * iInputRate; j += iInputRate )
    {
      dwColor = pCD->GetPixel( j, i );

      *pData++ = (BYTE)( ( dwColor & 0xff0000 ) >> 16 );
      *pData++ = (BYTE)( ( dwColor & 0xff00 ) >> 8 );
      *pData++ = (BYTE)( ( dwColor & 0xff ) );
    }
  }

  delete pCD;
  //filter.inRowBytes = iWidth * 3;//( filter.inHiPlane - filter.inLoPlane + 1 ) * 3;
  filter.inRowBytes = iWidth * ( filter.inHiPlane - filter.inLoPlane + 1 );
  //filter.outRowBytes = ( filter.outRect.right - filter.outRect.left + 1 ) * ( filter.outHiPlane - filter.outLoPlane + 1 ) * 3;
  filter.outRowBytes = ( filter.outRect.right - filter.outRect.left + 1 ) * ( filter.outHiPlane - filter.outLoPlane + 1 );

  /*
  filter.inRowBytes = iWidth * 3;
  filter.outRowBytes = ( filter.outRect.right - filter.outRect.left + 1 ) * 3;
  */

}





static OSErr
advance_state_proc (void)
{

  /*
  dh::Log( "advancestate" );

  dh::Log( "---------------" );
  dh::Log( "inRect %d,%d - %d,%d", 
           filter.inRect.left,
           filter.inRect.top,
           filter.inRect.right,
           filter.inRect.bottom );
  */

  PhotoShopFilterHandler.PSStoreResult( *PhotoShopFilterHandler.m_pFilter, PhotoShopFilterHandler.m_pTargetImage );

  PhotoShopFilterHandler.PSPrepareFilterRect( *PhotoShopFilterHandler.m_pFilter, PhotoShopFilterHandler.m_pWorkImage, PhotoShopFilterHandler.m_pTargetImage );

  if ( ( PhotoShopFilterHandler.m_pFilter->inRect.left == 0 )
  &&   ( PhotoShopFilterHandler.m_pFilter->inRect.top == 0 )
  &&   ( PhotoShopFilterHandler.m_pFilter->inRect.right == 0 )
  &&   ( PhotoShopFilterHandler.m_pFilter->inRect.bottom == 0 )
  &&   ( PhotoShopFilterHandler.m_pFilter->outRect.left == 0 )
  &&   ( PhotoShopFilterHandler.m_pFilter->outRect.top == 0 )
  &&   ( PhotoShopFilterHandler.m_pFilter->outRect.right == 0 )
  &&   ( PhotoShopFilterHandler.m_pFilter->outRect.bottom == 0 ) )
  {
    // der Filter will nichts mehr
    return noErr;
  }

  /*
  dh::Log( "outRect %d,%d - %d,%d", 
           filter.outRect.left,
           filter.outRect.top,
           filter.outRect.right,
           filter.outRect.bottom );
  */



  


  //dh::Log( "advancestate end" );

  /*
  // Ugly, ugly 
  static BOOL src_valid = FALSE, dst_valid = FALSE;
  static GimpPixelRgn src, dst;
  static Rect outRect;
  static WORD outRowBytes, outLoPlane, outHiPlane;
  
#ifdef PSPI_WITH_DEBUGGING
  if (debug_mask & PSPI_DEBUG_ADVANCE_STATE)
    {
      g_print (G_STRLOC ": in: ");
      PRINT_RECT (filter.inRect);
      g_print (",%d:%d", filter.inLoPlane, filter.inHiPlane);
      g_print ("\n  out: ");
      PRINT_RECT (filter.outRect);
      g_print (",%d:%d", filter.outLoPlane, filter.outHiPlane);
      g_print ("\n  inData=%p outData=%p drawable->bpp=%d\n",
	       filter.inData, filter.outData, drawable->bpp);
    }
#endif // PSPI_WITH_DEBUGGING 

  if (src_valid)
    {
      g_free (filter.inData);
      filter.inData = NULL;
      src_valid = FALSE;
    }

  if (dst_valid)
    {
      PSPI_DEBUG (ADVANCE_STATE, g_print ("  outData:\n"));
      store_buf ((BYTE *) filter.outData, outRowBytes, &dst, &outRect,
                 outLoPlane, outHiPlane);
      g_free (filter.outData);
      filter.outData = NULL;
      dst_valid = FALSE;
    }

  if (RECT_NONEMPTY (filter.inRect))
    {
      gimp_pixel_rgn_init (&src, drawable, filter.inRect.left, filter.inRect.top,
                           filter.inRect.right - filter.inRect.left,
                           filter.inRect.bottom - filter.inRect.top,
                           FALSE, FALSE);
      fill_buf ((BYTE **) &filter.inData, &filter.inRowBytes, &src,
                &filter.inRect, filter.inLoPlane, filter.inHiPlane);
      src_valid = TRUE;
    }
  else
    filter.inData = NULL;

  if (RECT_NONEMPTY (filter.outRect))
    {
      GimpPixelRgn src2;
      gimp_pixel_rgn_init (&src2, drawable, filter.outRect.left, filter.outRect.top,
                           filter.outRect.right - filter.outRect.left,
                           filter.outRect.bottom - filter.outRect.top,
                           FALSE, FALSE);
      gimp_pixel_rgn_init (&dst, drawable, filter.outRect.left, filter.outRect.top,
                           filter.outRect.right - filter.outRect.left,
                           filter.outRect.bottom - filter.outRect.top,
                           TRUE, TRUE);
      fill_buf ((BYTE **) &filter.outData, &filter.outRowBytes, &src,
                &filter.outRect, filter.outLoPlane, filter.outHiPlane);
      outRowBytes = filter.outRowBytes;
      outRect = filter.outRect;
      outLoPlane = filter.outLoPlane;
      outHiPlane = filter.outHiPlane;
      dst_valid = TRUE;
    }
  else
    filter.outData = NULL;

  */
  return noErr;
}


static OSErr
property_get_proc (PIType  signature,
                   PIType  key,
                   int32   index,
                   int32  *simpleProperty,
                   Handle *complexProperty)
{
  /*
  PSPI_DEBUG (PROPERTY_SUITE, g_print (G_STRLOC ": %s %s\n",
				       int32_as_be_4c (signature),
				       int32_as_be_4c (key)));
  */

  /*
  if (signature != kPhotoshopSignature)
    return errPlugInHostInsufficient;

  if (key == MULTIC (propNumberOfChannels))
    {
      WORD num_channels;
      int *channels = 1;//gimp_image_get_channels (image_id, &num_channels);
      g_free (channels);
      *simpleProperty = num_channels;
    }
  else if (key == MULTIC (propChannelName))
    {
      WORD num_channels;
      int *channels = 1;//gimp_image_get_channels (image_id, &num_channels);
      gchar *channel_name;
      gpointer p;

      if (index < 0 || index >= num_channels)
	{
	  g_free (channels);
	  return errPlugInPropertyUndefined;
	}
      channel_name = gimp_channel_get_name (channels[index]);
      g_free (channels);
      *complexProperty = handle_new_proc (strlen (channel_name));
      p = handle_lock_proc (*complexProperty, TRUE);
      memcpy (p, channel_name, strlen (channel_name));
      handle_unlock_proc (*complexProperty);
      g_free (channel_name);
    }
  else if (key == MULTIC (propImageMode))
    {
      *simpleProperty = filter.imageMode;
    }
  else if (key == MULTIC (propNumberOfPaths))
    {
      int i;
      WORD num_paths;
      gchar **paths = gimp_path_list (image_id, &num_paths);

      for (i = 0; i < num_paths; i++)
	g_free (paths[i]);
      g_free (paths);
      *simpleProperty = num_paths;
    }
  else if (key == MULTIC (propPathName))
    {
      int i;
      WORD num_paths;
      gchar **paths = gimp_path_list (image_id, &num_paths);
      gpointer p;

      if (index < 0 || index >= num_paths)
	{
	  for (i = 0; i < num_paths; i++)
	    g_free (paths[i]);
	  g_free (paths);
	  return errPlugInPropertyUndefined;
	}
      *complexProperty = handle_new_proc (strlen (paths[index]));
      p = handle_lock_proc (*complexProperty, TRUE);
      memcpy (p, paths[index], strlen (paths[index]));
      handle_unlock_proc (*complexProperty);
      for (i = 0; i < num_paths; i++)
	g_free (paths[i]);
      g_free (paths);
    }
  else
  */
    return errPlugInHostInsufficient;

  return noErr;
}


static OSErr
color_services_proc (ColorServicesInfo *info)
{

  //PSPI_DEBUG (COLOR_SERVICES_SUITE, PING ());
  
  if (info->infoSize < sizeof (ColorServicesInfo))
    return errPlugInHostInsufficient;

  switch (info->selector)
    {
    case plugIncolorServicesChooseColor: break;
    case plugIncolorServicesConvertColor: break;
    case plugIncolorServicesSamplePoint: break;
    case plugIncolorServicesGetSpecialColor: break;
    //default: g_assert_not_reached ();
    }

  //PSPI_DEBUG (COLOR_SERVICES_SUITE, PING ());

  return errPlugInHostInsufficient;
}






//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPhotoShopFilterHandler::CPhotoShopFilterHandler() :
  m_pWorkImage( NULL ),
  m_pTargetImage( NULL )
{

}

CPhotoShopFilterHandler::~CPhotoShopFilterHandler()
{

}



/*-PSReportError--------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CPhotoShopFilterHandler::PSReportError( FilterRecord& filter, int16 result )
{

  /* Many plug-ins seem to return error code 1 to indicate Cancel */
  if (result == userCanceledErr || result == 1)
    return;// GIMP_PDB_CANCEL;
  else
    {
      GR::tChar szTemp[10000];

      if (result == errReportString)
        {
          int msglen = (*filter.errorString)[0];
          for ( int i = 0; i < msglen; i++ )
          {
            szTemp[i] = (*filter.errorString)[i];
          }
          szTemp[msglen] = 0;
        }
      else
        {
          switch (result)
            {
            case readErr:
              wsprintf( szTemp, _T( "File read error" ) );
              break;
            case writErr:
              wsprintf( szTemp, _T( "File write error" ) );
              break;
            case openErr:
              wsprintf( szTemp, _T( "File open error" ) );
              break;
            case dskFulErr:
              wsprintf( szTemp, _T( "Disk full" ) );
              break;
            case ioErr:
              wsprintf( szTemp, _T( "File I/O error" ) );
              break;
            case memFullErr:
              wsprintf( szTemp, _T( "Out of memory" ) );
              break;
            case nilHandleErr:
              wsprintf( szTemp, _T( "Null handle error" ) );
              break;
            case filterBadParameters:
              wsprintf( szTemp, _T( "Bad parameters" ) );
              break;
            case filterBadMode:
              wsprintf( szTemp, _T( "Unsupported image mode" ) );
              break;
            case errPlugInHostInsufficient:
              wsprintf( szTemp, _T( "Requires services not provided by this host" ) );
              break;
            case errPlugInPropertyUndefined:
              wsprintf( szTemp, _T( "A requested property could not be found" ) );
              break;
            case errHostDoesNotSupportColStep:
              wsprintf( szTemp, _T( "This GIMP plug-in does not support colBytes other than 1" ) );
              break;
            case errInvalidSamplePoint:
              wsprintf( szTemp, _T( "Invalid sample point" ) );
              break;
            default:
              wsprintf( szTemp, _T( "error code %d" ), result);
              break;
            }
        }
      AfxMessageBox( szTemp );
    }
}



void CPhotoShopFilterHandler::OpenPhotoShopFilter( CViewInfo& viewInfo, const GR::String& strFilterFileName )
{
  BOOL    bFilterCancelled = FALSE;

  short   wResult = noErr;

  GR::Graphic::Image   *pImage = viewInfo.GetWorkImage();

  m_pWorkImage = pImage;

  m_pTargetImage = new GR::Graphic::Image( pImage->GetWidth(),
                                pImage->GetHeight(),
                                24 );

  int (MACPASCAL *plugin_ep)(  short  selector,
                              void  *pluginParamBlock,
                              long  *pluginData,
                              short  *result);


  HINSTANCE     hInstPlugin = NULL;

  HRSRC         hrSrc       = NULL;

  DWORD         dwSize      = 0;

  HGLOBAL       hGlob       = NULL;

  PIPropertyList    *pData      = NULL;




  //hInstPlugin = LoadLibrary( "d:\\projekte\\mfcpainter\\filter\\wovenwrd.8bf" );
  //hInstPlugin = LoadLibrary( "d:\\projekte\\mfcpainter\\filter\\halfwrap.8bf" );
  //hInstPlugin = LoadLibrary( "d:\\projekte\\mfcpainter\\filter\\gembar2.8bf" );
  //hInstPlugin = LoadLibrary( "d:\\projekte\\mfcpainter\\filter\\hg.8bf" );
  //hInstPlugin = LoadLibrary( "d:\\projekte\\mfcpainter\\filter\\c-cubes.8bf" );
  //hInstPlugin = LoadLibrary( "d:\\projekte\\mfcpainter\\filter\\icecream.8bf" );

  CPhotoShopFilterEntry* pFE = NULL;

  std::list<CPhotoShopFilterEntry>::iterator   it( pSettings->m_listPhotoShopFilter.begin() );

  while ( it != pSettings->m_listPhotoShopFilter.end() )
  {
    pFE = &(*it);
    if ( pFE->m_FileName.compare( strFilterFileName ) == 0 )
    {
      // der ist es!
      break;
    }

    it++;
  }
  if ( !pFE )
  {
    SafeDelete( m_pWorkImage );
    return;
  }

  hInstPlugin = LoadLibraryW( GR::Convert::ToUTF16( pFE->m_FileName ).c_str() );

  if ( hInstPlugin == NULL )
  {
    AfxGetMainWnd()->MessageBox( _T( "LoadLibrary failed" ) );
    goto cleanup;
  }

  // Proc-Address holen
  //if ( (plugin_ep = (int (__stdcall*)(const unsigned short,struct FilterRecord*,int*,unsigned short*))GetProcAddress( hInstPlugin, "ENTRYPOINT1" ))== NULL )
  if ( (plugin_ep = (int (__cdecl*)(short,void *,long*,short*))GetProcAddress( hInstPlugin, GR::Convert::ToStringA( pFE->m_strEntryPoint ).c_str() ) )== NULL )
  {
    AfxGetMainWnd()->MessageBox( _T( "GetProcAddress failed" ), GR::Convert::ToUTF16( pFE->m_strEntryPoint ).c_str() );
    goto cleanup;
  }

  {
    // About-Dialog vom Plugin
    /*
    {
      AboutRecord about;

        platform.hwnd = (long)AfxGetMainWnd()->GetSafeHwnd();
        about.platformData = &platform;
        about.sSPBasic = NULL // basic_suite
        about.plugInRef = NULL;
        memset (about.reserved, 0, sizeof (about.reserved));

        long lData = 0;

        short shResult = 0;

        shResult = noErr;
      plugin_ep( filterSelectorAbout, &about, &lData, &shResult );
    }
    */

    static FilterRecord   filter;

    m_pFilter = &filter;


    {
      

      ZeroMemory( &filter, sizeof( FilterRecord ) );
      
      long  dwData = 0;

      filter.imageSize.h = pImage->GetWidth();
      filter.imageSize.v = pImage->GetHeight();
      filter.planes = 3;
      filter.filterRect.left = 0;
      filter.filterRect.top = 0;
      filter.filterRect.right = pImage->GetWidth() - 1;
      filter.filterRect.bottom = pImage->GetHeight() - 1;

      filter.imageHRes = long2fixed ((long) (pImage->GetWidth() + 0.5));
      filter.imageVRes = long2fixed ((long) (pImage->GetHeight() + 0.5));


      filter.wholeSize.h = pImage->GetWidth();
      filter.wholeSize.v = pImage->GetHeight();

      filter.imageMode = plugInModeRGBColor;
      filter.inLayerPlanes = 3;
      filter.inTransparencyMask = 0;

      filter.serialNumber = 0;
      filter.abortProc = (unsigned char (__cdecl *)(void))PSAbortProc;
      filter.progressProc = progress_proc;
      filter.parameters = NULL;

      DWORD   dwBack = pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ),
              dwFore = pSettings->GetRGBColor( CSettings::CO_WORKCOLOR );

      int   red =   ( dwBack & 0xff0000 ) >> 16,
            green = ( dwBack & 0xff00 ) >> 8,
            blue = ( dwBack & 0xff );
      filter.background.red = (red * 65535) / 255;
      filter.background.green = (green * 65535) / 255;
      filter.background.blue = (blue * 65535) / 255;
      filter.backColor[0] = red;
      filter.backColor[1] = green;
      filter.backColor[2] = blue;
      filter.backColor[3] = 0xFF;

      red =   ( dwFore & 0xff0000 ) >> 16;
      green = ( dwFore & 0xff00 ) >> 8;
      blue = ( dwFore & 0xff );

      filter.foreground.red = (red * 65535) / 255;
      filter.foreground.green = (green * 65535) / 255;
      filter.foreground.blue = (blue * 65535) / 255;
      filter.foreColor[0] = red;
      filter.foreColor[1] = green;
      filter.foreColor[2] = blue;
      filter.foreColor[3] = 0xFF;

      filter.maxSpace = 100000000;
      memcpy ((char *) &filter.hostSig, "GRPT", 4);
      filter.hostProc = NULL;//host_proc;
      filter.platformData = &platform;
      filter.bufferProcs = buffer_procs;
      filter.resourceProcs = resource_procs;
      filter.processEvent = process_event_proc;
      filter.displayPixels = display_pixels_proc;
      filter.handleProcs = handle_procs;

      filter.supportsDummyChannels = FALSE; /* Docs say supportsDummyPlanes */
      filter.supportsAlternateLayouts = FALSE;
      filter.wantLayout = 0;
      filter.filterCase = 0;
      filter.dummyPlaneValue = -1;  
      /* premiereHook */
      filter.advanceState = advance_state_proc;
      filter.supportsAbsolute = TRUE;
      filter.wantsAbsolute = FALSE;
      filter.getPropertyObsolete = property_get_proc;
      /* cannotUndo */
      filter.supportsPadding = FALSE;
      /* inputPadding */
      /* outputPadding */
      /* maskPadding */
      filter.samplingSupport = FALSE;
      /* reservedByte */
      /* inputRate */
      /* maskRate */
      filter.colorServices = color_services_proc;

      filter.imageServicesProcs = image_services_procs;
      filter.propertyProcs = property_procs;
      /*
      filter.inTileHeight = gimp_tile_height ();
      filter.inTileWidth = gimp_tile_width ();
      filter.inTileOrigin.h = 0;
      filter.inTileOrigin.v = 0;
      filter.absTileHeight = filter.inTileHeight;
      filter.absTileWidth = filter.inTileWidth;
      filter.absTileOrigin.h = 0;
      filter.absTileOrigin.v = 0;   
      filter.outTileHeight = filter.inTileHeight;
      filter.outTileWidth = filter.inTileWidth;
      filter.outTileOrigin.h = 0;
      filter.outTileOrigin.v = 0;
      filter.maskTileHeight = filter.inTileHeight;
      filter.maskTileWidth = filter.inTileWidth;
      filter.maskTileOrigin.h = 0;
      filter.maskTileOrigin.v = 0;
      */

      filter.descriptorParameters = NULL /* descriptor_parameters */;
      filter.channelPortProcs = NULL /* channel_port_procs */;
      filter.documentInfo = NULL;

      filter.sSPBasic = NULL /* basic_suite */;
      filter.plugInRef = NULL;
      filter.depth = 8;

      filter.iCCprofileData = NULL;
      filter.iCCprofileSize = 0;
      filter.canUseICCProfiles = FALSE;

      memset (filter.reserved, 0, sizeof (filter.reserved));


      filter.inLayerMasks = 0;
      filter.inInvertedLayerMasks = 0;
      filter.inNonLayerPlanes = 0;

      filter.outLayerPlanes = filter.inLayerPlanes;
      filter.outTransparencyMask = filter.inTransparencyMask;
      filter.outLayerMasks = filter.inLayerMasks;
      filter.outInvertedLayerMasks = filter.inInvertedLayerMasks;
      filter.outNonLayerPlanes = filter.inNonLayerPlanes;

      filter.absLayerPlanes = filter.inLayerPlanes;
      filter.absTransparencyMask = filter.inTransparencyMask;
      filter.absLayerMasks = filter.inLayerMasks;
      filter.absInvertedLayerMasks = filter.inInvertedLayerMasks;
      filter.absNonLayerPlanes = filter.inNonLayerPlanes;

      filter.inPreDummyPlanes = 0;
      filter.inPostDummyPlanes = 0;
      filter.outPreDummyPlanes = 0;
      filter.outPostDummyPlanes = 0;

      filter.inColumnBytes = 0;
      filter.inPlaneBytes = 1;
      filter.outColumnBytes = 0;
      filter.outPlaneBytes = 1;

      filter.documentInfo = NULL /*make_read_image_document_desc ()*/;


      outRect.left = 0;
      outRect.top = 0;
      outRect.right = 0;
      outRect.bottom = 0;
      
      {
        // hmm, konnte nicht testen
        filter.parameters = NULL;

        plugin_ep( filterSelectorParameters, &filter, &dwData, &wResult );

        if ( wResult != noErr )
        {
          if ( ( wResult == userCanceledErr )
          ||   ( wResult == 1 ) )
          {
            bFilterCancelled = TRUE;
          }
          PSReportError( filter, wResult );
        }

        plugin_ep( filterSelectorPrepare, &filter, &dwData, &wResult );
        if ( wResult != noErr )
        {
          if ( ( wResult == userCanceledErr )
          ||   ( wResult == 1 ) )
          {
            bFilterCancelled = TRUE;
          }
          PSReportError( filter, wResult );
        }

        plugin_ep( filterSelectorStart, &filter, &dwData, &wResult );
        if ( wResult != noErr )
        {
          if ( ( wResult == userCanceledErr )
          ||   ( wResult == 1 ) )
          {
            bFilterCancelled = TRUE;
          }
          PSReportError( filter, wResult );
        }

        while ( wResult == noErr )
        {
          // jetzt müßten In- und OutRect Werte anzeigen
          /*
          dh::Log( "-stdloop--------------" );
          dh::Log( "inRect %d,%d - %d,%d", 
                   filter.inRect.left,
                   filter.inRect.top,
                   filter.inRect.right,
                   filter.inRect.bottom );
          dh::Log( "outRect %d,%d - %d,%d", 
                   filter.outRect.left,
                   filter.outRect.top,
                   filter.outRect.right,
                   filter.outRect.bottom );
          */

          PSStoreResult( filter, m_pTargetImage );

          PSPrepareFilterRect( filter, pImage, m_pTargetImage );

          if ( ( filter.inRect.left == 0 )
          &&   ( filter.inRect.top == 0 )
          &&   ( filter.inRect.right == 0 )
          &&   ( filter.inRect.bottom == 0 )
          &&   ( filter.outRect.left == 0 )
          &&   ( filter.outRect.top == 0 )
          &&   ( filter.outRect.right == 0 )
          &&   ( filter.outRect.bottom == 0 ) )
          {
            // der Filter will nichts mehr
            break;
          }
          

          
          
          plugin_ep( filterSelectorContinue, &filter, &dwData, &wResult );
        }

        if ( wResult != noErr )
        {
          if ( ( wResult == userCanceledErr )
          ||   ( wResult == 1 ) )
          {
            bFilterCancelled = TRUE;
          }
          PSReportError( filter, wResult );
        }

        if ( filter.inData )
        {
          delete[] filter.inData;
          filter.inData = NULL;
        }
        if ( filter.outData )
        {
          delete[] filter.outData;
          filter.outData = NULL;
        }

        if ( wResult != noErr )
        {
          if ( ( wResult == userCanceledErr )
          ||   ( wResult == 1 ) )
          {
            bFilterCancelled = TRUE;
          }
          PSReportError( filter, wResult );
        }

        plugin_ep( filterSelectorFinish, &filter, &dwData, &wResult );
      }
    }
  }

  cleanup:;

  // DLL rauswerfen
  if ( hInstPlugin )
  {
    FreeLibrary( hInstPlugin );
    hInstPlugin = NULL;
  }

  // und das Ergebnis in die Grafik übertragen
  if ( ( !bFilterCancelled )
  &&   ( wResult == noErr ) )
  {
    CMaskedContextDescriptor  *pCDFilterTarget = new CMaskedContextDescriptor( m_pTargetImage, NULL ),
                        *pCDTarget = viewInfo.GetCD();

    if ( pCDTarget )
    {
      int     iXOffset = 0,
              iYOffset = 0;

      if ( viewInfo.m_pDocInfo->HasSelection() )
      {
        iXOffset = viewInfo.m_pDocInfo->m_Selection.left;
        iYOffset = viewInfo.m_pDocInfo->m_Selection.top;
      }

      viewInfo.m_pDocInfo->AddUndoRect( 0, 0, viewInfo.m_pDocInfo->Width() - 1, viewInfo.m_pDocInfo->Height() - 1, -1, -1, false );

      for ( int i = 0; i < m_pTargetImage->GetWidth(); i++ )
      {
        for ( int j = 0; j < m_pTargetImage->GetHeight(); j++ )
        {
          pCDTarget->PutPixel( iXOffset + i, iYOffset + j, pCDFilterTarget->GetPixel( i, j ) );
        }
      }
    }
    SafeDelete( pCDFilterTarget );
    SafeDelete( pCDTarget );
    SafeDelete( m_pTargetImage );

    viewInfo.m_pDocInfo->RedrawAllViews();
  }

  SafeDelete( m_pWorkImage );

}



