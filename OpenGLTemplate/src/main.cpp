/*

	@author: Xiangshun Bei(bxs3514)
	@date: 07/20/2016
*/

#include "OpenGLTemplate.h"

int main()
{
	Setting setting;
    setting.m_graphicsSetting.DisableSSAO();

    OpenGLTemplate m_oglTemplate(&setting);
    m_oglTemplate.run();

	return 0;
}
