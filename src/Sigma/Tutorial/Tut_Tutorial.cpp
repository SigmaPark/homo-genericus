#include "Tutorial.hpp"
#include "Tut_Tutorial.hpp"


using namespace sgm::tut;


static void intro()
{
	mdo
	<<	"# Title\n"
	<<	"## subtitle\n"
	<<	"What is the integer sum from 1 to 10?  \n"
	<<	"Gauss solved this when he was in elementary school.  \n"
	<<	"\
		Now,  \n\
		let's check  \n\
		how to implement it  \n\
		by C++  \n\
		"_mdo;
}


static void Ex01()
{
BEGIN_CODE_BLOCK(1)
	int sum = 0;

	for(int i = 1;  i <= 10;  ++i)
		sum += i;

	is_True(sum == 55);
END_CODE_BLOCK_AND_LOAD(1)
}


static void outro()
{
	mdo << "---" << '\n';

	mdo << LOAD_DESCRIPTION_FILE("YOLO.txt");
	
	{
		html_block_guard bg("strong center");

		mdo << "Thank you for watching";
	}	
	{
		html_block_guard bg("center");

		mdo << Load_image("sample_img.png", 200);
	}
}


SGM_DEFAULT_TUTORIAL(sgm_Tutorial)
{	::intro
,	::Ex01
,	::outro
};