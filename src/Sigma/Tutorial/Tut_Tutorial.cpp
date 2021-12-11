#include "Tutorial.hpp"
#include "Tut_Tutorial.hpp"


static void intro()
{
	sgm::tut::mdo
	<<	"# Introduction\n"
	<<	"Tutorial library helps you make tutorial documents for a code library with its example codes.  \n"
	<<	"You can easily create the tutorial documents as Markdown format just by writing example codes."
	<<	sgm::tut::empty_line;
}


static void Math_Expression()
{
	sgm::tut::mdo 
	<<	"# Math Expression\n"
	<<	"Markdown supports writing mathematical expressions using LaTeX syntex.  \n"
	<<	R"($$ \sum_{n = 1}^{\infty}{n^{-2}} = \frac{\pi^{2}}{6} $$)"
	<<	sgm::tut::empty_line;
}


static void Code_Block()
{
	sgm::tut::mdo
	<<	"# Code Block\n"
	<<	"Macro \"BEGIN_CODE_BLOCK\" and \"END_CODE_BLOCK\" captures codes.  \n";

BEGIN_CODE_BLOCK(1)
	int sum = 0;

	for(int i = 1;  i <= 10;  ++i)
		sum += i;

	sgm::tut::is_True(sum == 55);
END_CODE_BLOCK(1)

	sgm::tut::mdo << LOAD_CODE_BLOCK(1) << sgm::tut::empty_line;
}


static void Assertion()
{
	sgm::tut::mdo
	<<	"# sgm::tut::is_True\n"
	<<	"You can not only show codes but also assert the result from the code is correct using "
	<<	"\"sgm::tut::is_True\"  \n";

BEGIN_CODE_BLOCK(2)
	int result = 1 + 1;

	sgm::tut::is_True(result == 2);
END_CODE_BLOCK_AND_LOAD(2)

	sgm::tut::mdo
	<<	"\"sgm::tut::is_True\" throws an std::exception if the boolean parameter is false, "
	<<	"and the exception prevents making Markdown file export. Therefore the code result is correct "
	<<	"if the document file is successfully created."
	<<	sgm::tut::empty_line;
}


static void External_Resources()
{
	sgm::tut::mdo
	<<	"# External Resources\n"
	<<	"All external materials you want to attach to a document are to be located at \"\\md_materials\"  \n\n"
	<<	"## Text File\n"
	<<	"Use a macro function \"LOAD_DESCRIPTION_FILE\"  \n"
	<<	LOAD_CODE_BLOCK(3) << sgm::tut::newl;


	{
		sgm::tut::md_block_guard mbg;

BEGIN_CODE_BLOCK(3)
		sgm::tut::mdo << LOAD_DESCRIPTION_FILE("YOLO.txt");
END_CODE_BLOCK(3)
	}


	sgm::tut::mdo
	<<	sgm::tut::newl
	<<	"## Image File\n"
	<<	"Use \"sgm::tut::Load_image\" function.  \n" << LOAD_CODE_BLOCK(4) << sgm::tut::newl;


BEGIN_CODE_BLOCK(4)
	{
		sgm::tut::html_block_guard hbg("center");

		sgm::tut::mdo << sgm::tut::Load_image("sample_img.png", 200);
	}
END_CODE_BLOCK(4)


	sgm::tut::mdo
	<<	sgm::tut::newl
	<<	"The 2nd parameter means image width to show and can be omitted(then it shows the image by original size)."
	<<	sgm::tut::empty_line;
}


static void Guards()
{
	sgm::tut::mdo
	<<	"# Guards\n"
	<<	"## Block Guard\n" << LOAD_CODE_BLOCK(5) << sgm::tut::newl;

BEGIN_CODE_BLOCK(5)
	{
		sgm::tut::md_block_guard mbg;

		sgm::tut::mdo 
		<<	"Until \"mbg\" is alive(before its destructor is excuted), all contents are in box tab.  \n";
	}
END_CODE_BLOCK(5)

	sgm::tut::mdo
	<<	sgm::tut::newl
	<<	"## HTML Guard\n";



}


static void outro()
{
	{
		sgm::tut::html_block_guard bg("strong center");

		sgm::tut::mdo << "Thank you for watching";
	}	
}


SGM_DEFAULT_TUTORIAL(sgm_Tutorial)
{	::intro
,	::Math_Expression
,	::Code_Block
,	::Assertion
,	::External_Resources
,	::Guards
,	::outro
};