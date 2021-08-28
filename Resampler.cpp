#include <Grafik/ContextDescriptor.h>

#include <vector>

#include ".\resampler.h"



namespace Resampler
{

  class IFilter
  {

    public:

      double            m_defaultFilterRadius;


      virtual double    GetValue( double x ) = 0;

  };


  class HermiteFilter : public IFilter 
  {

    public:

      HermiteFilter() 
      {
        m_defaultFilterRadius = 1;
      }

      virtual double GetValue( double x ) 
      {
        if ( x < 0 )
        {
          x = - x;
        }
        if ( x < 1 )
        {
          return ( ( 2 * x - 3 ) * x * x + 1 );
        }
        return 0;
      }

  };

  class BoxFilter : public IFilter 
  {

    public:

      BoxFilter() 
      {
        m_defaultFilterRadius = 0.5;
      }

      virtual double GetValue( double x ) 
      {
        if ( x < 0 )
        {
          x = - x;
        }
        if ( x <= 0.5 )
        {
          return 1;
        }
        return 0;
      }

  };

  class TriangleFilter : public IFilter  
  {

    public:

      TriangleFilter() 
      {
        m_defaultFilterRadius = 1;
      }

      virtual double GetValue( double x ) 
      {
        if ( x < 0 )
        {
          x = - x;
        }
        if ( x < 1 )
        {
          return( 1 - x );
        }
        return 0;
      }
  };

  class BellFilter : public IFilter 
  {

    public:

      BellFilter() 
      {
        m_defaultFilterRadius = 1.5;
      }

      virtual double GetValue( double x ) 
      {
        if ( x < 0 )
        {
          x = - x;
        }
        if ( x < 0.5 )
        {
          return( 0.75 - x * x );
        }
        if ( x < 1.5 )
        {
          return ( 0.5 * pow( x - 1.5, 2 ) );
        }
        return 0;
      }

  };

  class CubicBSplineFilter : public IFilter  
  {

    public:

      double temp;


      CubicBSplineFilter() 
      {
        m_defaultFilterRadius = 2;
      }

      virtual double GetValue( double x ) 
      {
        if ( x < 0 )
        {
          x = - x;
        }
        if ( x < 1 ) 
        {
          temp = x * x;
          return( 0.5 * temp * x - temp + 2.0 / 3.0 );
      }
      if ( x < 2 ) 
      {
        x = 2.0 - x;
        return( pow( x, 3 ) / 6.0 );
      }
      return 0;
    }

  };

  class Lanczos3Filter : public IFilter  
  {

    public:

      Lanczos3Filter() 
      {
        m_defaultFilterRadius = 3;
      }

      double SinC( double x ) 
      {
        if ( x != 0 ) 
        {
          x *= 3.1415926;
          return( sin( x ) / x );
        }
        return 1;
      }

      virtual double GetValue( double x ) 
      {
        if ( x < 0 )
        {
          x = - x;
        }
        if ( x < 3 )
        {
          return( SinC( x ) * SinC( x / 3.0 ) );
        }
        return 0;
      }

  };

  class MitchellFilter : public IFilter  
  {

    public:

      double    C;
      double    temp;

      MitchellFilter() 
      {
        C = 1.0 / 3.0;
        m_defaultFilterRadius = 2;
      }

      virtual double GetValue( double x ) 
      {
        if ( x < 0 )
        {
          x = - x;
        }
        temp = x * x;
        if ( x < 1 ) 
        {
          x = ( ( ( 12 - 9 * C - 6 * C ) * ( x * temp ) ) + ( ( -18 + 12 * C + 6 * C ) * temp ) + ( 6 - 2 * C ) );
          return( x / 6 );
        }
        if ( x < 2 ) 
        {
          x = ( ( ( -C - 6 * C ) * ( x * temp ) ) + ( ( 6 * C + 30 * C ) * temp ) + ( ( -12 * C - 48 * C ) * x ) + ( 8 * C + 24 * C ) );
          return( x / 6 );
        }
        return 0;
      }
  };

  class CosineFilter : public IFilter  
  {

    public:

      CosineFilter() 
      {
        m_defaultFilterRadius = 1;
      }

      virtual double GetValue( double x ) 
      {
        if ( ( x >= -1 ) 
        &&   ( x <= 1 ) ) 
        {
          return( ( cos( x * 3.1415926 ) + 1 ) / 2.0 );
        }
        return 0;
      }
  };

  class CatmullRomFilter : public IFilter  
  {

    public:

      double C;
      double temp;

      CatmullRomFilter() 
      {
        C = 1.0 / 2.0;
        m_defaultFilterRadius = 2;
      }

      virtual double GetValue( double x ) 
      {
        if ( x < 0 )
        {
          x = - x;
        }
        temp = x * x;
        if ( x <= 1 )
        {
          return( 1.5 * temp * x - 2.5 * temp + 1 );
        }
        if ( x <= 2 )
        {
          return( -0.5 * temp * x + 2.5 * temp - 4 * x + 2 );
        }
        return 0;
      }
  };

  class QuadraticFilter : public IFilter  
  {

    public:

      QuadraticFilter() 
      {
        m_defaultFilterRadius = 1.5;
      }

      virtual double GetValue( double x ) 
      {
        if ( x < 0 )
        {
          x = - x;
        }
        if ( x <= 0.5 )
        {
          return( -2 * x * x + 1 );
        }
        if ( x <= 1.5 )
        {
          return( x * x - 2.5 * x + 1.5 );
        }
        return 0;
      }
  };

  class QuadraticBSplineFilter : public IFilter  
  {

    public:

      QuadraticBSplineFilter() 
      {
        m_defaultFilterRadius = 1.5;
      }

      virtual double GetValue( double x ) 
      {
        if ( x < 0 )
        {
          x = - x;
        }
        if ( x <= 0.5 )
        {
          return( -x * x + 0.75 );
        }
        if ( x <= 1.5 )
        {
          return( 0.5 * x * x - 1.5 * x + 1.125 );
        }
        return 0;
      }
  };

  class CubicConvolutionFilter : public IFilter  
  {

    public:

      double temp;

      CubicConvolutionFilter() 
      {
        m_defaultFilterRadius = 3;
      }

      virtual double GetValue( double x ) 
      {
        if ( x < 0 )
        {
          x = - x;
        }
        temp = x * x;
        if ( x <= 1 )
        {
          return ( ( 4.0 / 3.0 ) * temp * x - ( 7.0 / 3.0 ) * temp + 1 );
        }
        if ( x <= 2 )
        {
          return( -( 7.0 / 12.0 ) * temp * x + 3 * temp - ( 59.0 / 12.0 ) * x + 2.5 );
        }
        if ( x <= 3 )
        {
          return( ( 1.0 / 12.0 ) * temp * x - ( 2.0 / 3.0 ) * temp + 1.75 * x - 1.5 );
        }
        return 0;
      }

  };

  class Lanczos8Filter : public IFilter  
  {   

    public:

      Lanczos8Filter() 
      {
        m_defaultFilterRadius = 8;
      }

      double SinC( double x ) 
      {
        if ( x != 0 ) 
        {
          x *= 3.1415926;
          return( sin( x ) / x );
        }
        return 1;
      }

      virtual double GetValue( double x ) 
      {
        if ( x < 0 )
        {
          x = - x;
        }
        if ( x < 8 )
        {
          return( SinC( x ) * SinC( x / 8.0 ) );
        }
        return 0;
      }

  };

  struct Contributor 
  {
    int     pixel;
    double  weight;
  };

  struct ContributorEntry 
  {
    int                           n;
    std::vector<Contributor>      p;
    double                        wsum;
  };

  bool Resample( const GR::Graphic::ContextDescriptor& cdIn, GR::Graphic::ContextDescriptor& cdOut, const eSampleMethod Method )
  {
    if ( ( cdIn.Width() == 0 )
    ||   ( cdIn.Height() == 0 )
    ||   ( cdOut.Width() == 0 )
    ||   ( cdOut.Height() == 0 ) )
    {
      return false;
    }

    IFilter*      pFilter = NULL;

    switch ( Method )
    {
      case SM_BELL:
        pFilter = new BellFilter();
        break;
      case SM_BOX:
        pFilter = new BoxFilter();
        break;
      case SM_CATMULLROM:
        pFilter = new CatmullRomFilter();
        break;
      case SM_COSINE:
        pFilter = new CosineFilter();
        break;
      case SM_CUBICBSPLINE:
        pFilter = new CubicBSplineFilter();
        break;
      case SM_CUBICCONVOLUTION:
        pFilter = new CubicConvolutionFilter();
        break;
      case SM_HERMITE:
        pFilter = new HermiteFilter();
        break;
      case SM_LANCZOS3:
        pFilter = new Lanczos3Filter();
        break;
      case SM_LANCZOS8:
        pFilter = new Lanczos8Filter();
        break;
      case SM_MITCHELL:
        pFilter = new MitchellFilter();
        break;
      case SM_QUADRATIC:
        pFilter = new QuadraticFilter();
        break;
      case SM_QUADRATICBSPLINE:
        pFilter = new QuadraticBSplineFilter();
        break;
      case SM_TRIANGLE:
        pFilter = new TriangleFilter();
        break;
      default:
        return false;
    }

    std::vector<ContributorEntry>   contrib;


    int current = 0;
    int total   = cdOut.Width() + cdIn.Height();

    // set operating pixel formating
    // create intermediate image to hold horizontal zoom
    //Bitmap work = new Bitmap(cdOut.Width(), cdIn.Height(), pf);

    GR::Graphic::ContextDescriptor    cdWork;
    
    cdWork.CreateData( cdOut.Width(), cdIn.Height(), cdIn.ImageFormat() );

    /*
    // lock bitmaps
    bdIn = input.LockBits(new Rectangle(0, 0, cdIn.Width(), cdIn.Height()),
      ImageLockMode.ReadOnly, pf);
    bdOut = work.LockBits(new Rectangle(0, 0, cdOut.Width(), cdIn.Height()),
      ImageLockMode.ReadWrite, pf);
      */

    double xScale = (double)cdOut.Width() / (double)cdIn.Width();
    double yScale = (double)cdOut.Height() / (double)cdIn.Height();

    contrib.resize( cdOut.Width() ); 

    // horizontal downsampling
    if ( xScale < 1 ) 
    {
      // scales from bigger to smaller width
      double wdth = pFilter->m_defaultFilterRadius / xScale;
      for ( int i = 0; i < cdOut.Width(); i++ ) 
      {
        contrib[i].n = 0;
        contrib[i].p.resize( (int)floor( 2 * wdth + 1 ) );
        contrib[i].wsum = 0;
        double center = ( i + 0.5 ) / xScale;
        int left = (int)( center - wdth );
        int right = (int)( center + wdth );
        for ( int j = left; j <= right; j++ ) 
        {
          double weight = pFilter->GetValue( ( center - j - 0.5 ) * xScale );
          if ( ( weight == 0 ) 
          ||   ( j < 0 ) 
          ||   ( j >= cdIn.Width() ) )
          {
            continue;
          }
          contrib[i].p[contrib[i].n].pixel = j;
          contrib[i].p[contrib[i].n].weight = weight;
          contrib[i].wsum += weight;
          contrib[i].n++;
        }
      }
    } 
    else 
    {
      // horizontal upsampling
      // scales from smaller to bigger width
      for ( int i = 0; i < cdOut.Width(); i++ )
      {
        contrib[i].n = 0;
        contrib[i].p.resize( (int)floor( 2 * pFilter->m_defaultFilterRadius + 1 ) );
        contrib[i].wsum = 0;
        double center = ( i + 0.5 ) / xScale;
        int left = (int)floor( center - pFilter->m_defaultFilterRadius );
        int right = (int)ceil( center + pFilter->m_defaultFilterRadius );
        for ( int j = left; j <= right; j++ ) 
        {
          double weight = pFilter->GetValue( center - j - 0.5 );
          if ( ( weight == 0 ) 
          ||   ( j < 0 ) 
          ||   ( j >= cdIn.Width() ) )
          {
            continue;
          }
          contrib[i].p[contrib[i].n].pixel = j;
          contrib[i].p[contrib[i].n].weight = weight;
          contrib[i].wsum += weight;
          contrib[i].n++;
        }
      }
    }

    // filter horizontally from input to work
    for ( int k = 0; k < cdIn.Height(); k++ ) 
    {
      //rowIn = (byte*)bdIn.Scan0 + k*bdIn.Stride;
      //rowOut = (byte*)bdOut.Scan0 + k*bdOut.Stride;
      for ( int i = 0; i < cdOut.Width(); i++ ) 
      {
        double  r = 0,
                g = 0,
                b = 0;

        for ( int j = 0; j < contrib[i].n; j++ ) 
        {
          int offset = contrib[i].p[j].pixel;
          double weight = contrib[i].p[j].weight;
          if ( weight == 0 )
          {
            continue;
          }

          r += ( ( cdIn.GetPixel( offset, k ) & 0xff0000 ) >> 16 ) * weight;
          g += ( ( cdIn.GetPixel( offset, k ) & 0x00ff00 ) >> 8 ) * weight;
          b +=   ( cdIn.GetPixel( offset, k ) & 0x0000ff ) * weight;
        }
        r /= contrib[i].wsum;
        g /= contrib[i].wsum;
        b /= contrib[i].wsum;

        GR::u32     dwOut = 0;

        if ( r > 255 )
        {
          dwOut |= 0xff0000;
        }
        else if ( r > 0 )
        {
          dwOut |= ( (GR::u8)r ) << 16;
        }
        if ( g > 255 )
        {
          dwOut |= 0x00ff00;
        }
        else if ( g > 0 )
        {
          dwOut |= ( (GR::u8)g ) << 8;
        }
        if ( b > 255 )
        {
          dwOut |= 0x0000ff;
        }
        else if ( b > 0 )
        {
          dwOut |= ( (GR::u8)b );
        }
        cdWork.PutPixel( i, k, dwOut );
      }

      current++;
    }

    // pre-calculate filter contributions for a column
    contrib.resize( cdOut.Height() );
    // vertical downsampling
    if ( yScale < 1 ) 
    {

      // scales from bigger to smaller height
      double wdth = pFilter->m_defaultFilterRadius / yScale;
      for ( int i = 0; i < cdOut.Height(); i++ ) 
      {
        contrib[i].n = 0;
        contrib[i].p.resize( (int)floor( 2 * wdth + 1 ) );
        contrib[i].wsum = 0;
        double center = ( i + 0.5 ) / yScale;
        int left = (int)( center - wdth );
        int right = (int)( center + wdth );
        for ( int j = left; j <= right; j++ ) 
        {
          double weight = pFilter->GetValue( ( center - j - 0.5 ) * yScale );
          if ( ( weight == 0 ) 
          ||   ( j < 0 ) 
          ||   ( j >= cdWork.Height() ) )
          {
            continue;
          }
          contrib[i].p[contrib[i].n].pixel = j;
          contrib[i].p[contrib[i].n].weight = weight;
          contrib[i].wsum += weight;
          contrib[i].n++;
        }
      }
    } 
    else 
    {
      // vertical upsampling
      // scales from smaller to bigger height
      for ( int i = 0; i < cdOut.Height(); i++ ) 
      {
        contrib[i].n = 0;
        contrib[i].p.resize( (int)floor( 2 * pFilter->m_defaultFilterRadius + 1 ) );
        contrib[i].wsum = 0;
        double center = ( i + 0.5 ) / yScale;
        int left = (int)( center - pFilter->m_defaultFilterRadius );
        int right = (int)( center + pFilter->m_defaultFilterRadius );
        for ( int j = left; j <= right; j++ ) 
        {
          double weight = pFilter->GetValue( center - j - 0.5 );
          if ( ( weight == 0 ) 
          ||   ( j < 0 ) 
          ||   ( j >= cdWork.Height() ) )
          {
            continue;
          }
          contrib[i].p[contrib[i].n].pixel = j;
          contrib[i].p[contrib[i].n].weight = weight;
          contrib[i].wsum += weight;
          contrib[i].n++;
        }
      }
    }

    // filter vertically from work to output
    for ( int k = 0; k < cdOut.Width(); k++ ) 
    {
      for ( int i = 0; i < cdOut.Height(); i++ ) 
      {
        double  r = 0, 
                g = 0, 
                b = 0;

        for ( int j = 0; j < contrib[i].n; j++ ) 
        {
          double weight = contrib[i].p[j].weight;
          if ( weight == 0 )
          {
            continue;
          }

          r += ( ( cdWork.GetPixel( k, contrib[i].p[j].pixel ) & 0xff0000 ) >> 16 ) * weight;
          g += ( ( cdWork.GetPixel( k, contrib[i].p[j].pixel ) & 0x00ff00 ) >> 8 ) * weight;
          b +=   ( cdWork.GetPixel( k, contrib[i].p[j].pixel ) & 0x0000ff ) * weight;
        }

        r /= contrib[i].wsum;
        g /= contrib[i].wsum;
        b /= contrib[i].wsum;

        GR::u32     dwOut = 0;

        if ( r > 255 )
        {
          dwOut |= 0xff0000;
        }
        else if ( r > 0 )
        {
          dwOut |= ( (GR::u8)r ) << 16;
        }
        if ( g > 255 )
        {
          dwOut |= 0x00ff00;
        }
        else if ( g > 0 )
        {
          dwOut |= ( (GR::u8)g ) << 8;
        }
        if ( b > 255 )
        {
          dwOut |= 0x0000ff;
        }
        else if ( b > 0 )
        {
          dwOut |= ( (GR::u8)b );
        }
        cdOut.PutPixel( k, i, dwOut );
      }

      current++;
    }

    delete pFilter;

    return true;
  }

};