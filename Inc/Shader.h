#ifndef _SHADER_H_
#define _SHADER_H_


#include "Common.h"


class Shader
{
  Shader();
  ^Shader();
  
  bool CreateShader(PS,VS,Layout);
  
  private:
   struct ShaderData
   {
   PixelShader;
   VertexShader;
   InputLayout;
   }
}


#endif // !_SHADER_H_
