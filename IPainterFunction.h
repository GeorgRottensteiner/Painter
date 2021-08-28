#ifndef _IPAINTER_FUNCTION_H_
#define _IPAINTER_FUNCTION_H_

class CViewInfo;

struct IPainterFunction
{

  virtual void      ApplyAt( CViewInfo* pViewInfo, int iX, int iY ) = 0;

};

#endif // _IPAINTER_FUNCTION_H_