#ifndef FORMAT_PNT_H
#define FORMAT_PNT_H

#pragma once

#include <GR/GRTypes.h>


class DocumentInfo;
struct IIOStream;



bool SavePNT( DocumentInfo& DocInfo, const GR::String& Filename );
bool SavePNT( DocumentInfo& DocInfo, IIOStream& IOOut );

bool LoadPNT( DocumentInfo& DocInfo, const GR::String& Filename );
bool LoadPNT( DocumentInfo& DocInfo, IIOStream& IOIn );


#endif // FORMAT_PNT_H