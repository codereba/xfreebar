/*
 * Copyright 2010 www.codereba.com
 *
 * This file is part of xfreebar.
 *
 * xfreebar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * xfreebar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xfreebar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "stdafx.h"

/*
** If you don't want to encode a character class considered reserved
** or unsafe by the rfc1738, just supply the classes you don't want
** encoded or'ing them with the url_normal class. If you want to encode
** every character of the supplied string, just use url_none. Otherwise
** just use the default parameter and the string will be encoded
** properly.
*/
const XString urlencode(const XString& _s, INT32 _flags)
{
  const static unsigned char m[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    2,1,2,2,1,2,4,2,1,1,1,2,1,1,1,4,1,1,1,1,1,1,1,1,1,1,4,4,2,4,2,4,
                //1       1                                
    4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,1,
    2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  };
  const static char* h = "0123456789ABCDEF";
  XString encoded;
  for(XString::const_iterator i=_s.begin(); i<_s.end(); i++)
  {
    unsigned char ii = *i;
    if(ii>0xFF || m[ii]&_flags)
      encoded += ii;
    else {
      encoded += '%';
      encoded += h[ii>>4];
      encoded += h[ii&0x0F];
    }
  }
  return encoded;
}