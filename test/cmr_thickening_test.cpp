#include "gtest/gtest.h"
#include "complext.h"
#include "hoNDArray_reductions.h"
#include "hoNDArray_elemwise.h"
#include "hoNFFT.h"
#include "cmr_radial_thickening.h"
#include "ImageIOAnalyze.h"
#include "GadgetronTimer.h"

using namespace Gadgetron;
using testing::Types;

template<typename Real>
class cmr_thickening_test : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		GDEBUG_STREAM("=============================================================================================");
		char* ut_dir = ::getenv("GT_UNITTEST_DIRECTORY");
		if (ut_dir != NULL)
		{
			gt_ut_folder_ = std::string(ut_dir);
		}
		else
			gt_ut_folder_.clear();

		GDEBUG_STREAM("=============================================================================================");

		if (!gt_ut_folder_.empty())
		{
			GDEBUG_STREAM("Unit Test for Gadgetron hoNFFT");
			gt_ut_data_folder_ = gt_ut_folder_;
			gt_ut_res_folder_ = gt_ut_folder_ + "/result/";
			GDEBUG_STREAM("gt_ut_data_folder_ is " << gt_ut_data_folder_);
			GDEBUG_STREAM("gt_ut_res_folder_ is " << gt_ut_res_folder_);
		}

		this->timer_.set_timing_in_destruction(false);
	}

	std::string gt_ut_folder_;
	std::string gt_ut_data_folder_;
	std::string gt_ut_res_folder_;

	ImageIOAnalyze gt_io_;
	GadgetronTimer timer_;
};

typedef Types<float> realImplementations;

TYPED_TEST_CASE(cmr_thickening_test, realImplementations);

TYPED_TEST(cmr_thickening_test, 229201050_229201055_362_20190718_retro_cine)
{
	typedef float T;

	if (this->gt_ut_folder_.empty())
	{
		GDEBUG_STREAM("Gadgetron unit test directory is not set ... ");
		return;
	}

	/*hoNDArray<T> res, diffs;
	hoNDArray<T> refl(5);
	refl(0) = 1;
	refl(1) = 2;
	refl(2) = 3;
	refl(3) = 4;
	refl(4) = 5;
	T start = 1;
	T end = 5; 
	int num = 5;
	Gadgetron::linspace(start, end, num, res);
	Gadgetron::subtract(refl, res, diffs);
	T l = Gadgetron::nrm2(diffs);
	T norm_l = Gadgetron::nrm2(refl);
	EXPECT_LE(l / norm_l, 0.002);
*/
	hoNDArray< T > epi_mask;
	this->gt_io_.import_array(epi_mask, this->gt_ut_data_folder_ + "/Masks/retro_cine_epi_mask");
	epi_mask.print(std::cout);

	hoNDArray< T > endo_mask;
	this->gt_io_.import_array(endo_mask, this->gt_ut_data_folder_ + "/Masks/retro_cine_endo_mask");
	endo_mask.print(std::cout);

	hoNDArray<T> rad_strain, edge_endo, edge_epi;
	size_t ref_phase = 0;
	Gadgetron::compute_thickening(endo_mask, epi_mask, ref_phase, edge_endo, edge_epi, rad_strain);

	this->gt_io_.export_array(rad_strain, this->gt_ut_res_folder_ + "/Thickening/retro_cine_strain");
	this->gt_io_.export_array(edge_endo, this->gt_ut_res_folder_ + "/Thickening/retro_edge_endo");
	this->gt_io_.export_array(edge_epi, this->gt_ut_res_folder_ + "/Thickening/retro_edge_epi");

	// compare agains ground truth
	hoNDArray<T> ref;
	hoNDArray<T> diff;
	T norm_ref;

	this->gt_io_.import_array(ref, this->gt_ut_data_folder_ + "/Masks/retro_cine_strain");
	Gadgetron::subtract(ref, rad_strain, diff);
	T v = Gadgetron::nrm2(diff);
	norm_ref = Gadgetron::nrm2(ref);
	std::cout << v << std::endl;
	std::cout << norm_ref << std::endl;
	std::cout << v / norm_ref << std::endl;
	EXPECT_LE(v / norm_ref, 0.18);

	this->gt_io_.import_array(ref, this->gt_ut_data_folder_ + "/Masks/retro_cine_edge_endo");
	Gadgetron::subtract(ref, edge_endo, diff);
	T q = Gadgetron::nrm2(diff);
	norm_ref = Gadgetron::nrm2(ref);
	std::cout << q / norm_ref << std::endl;
	EXPECT_LE(q / norm_ref, 0.15);

	this->gt_io_.import_array(ref, this->gt_ut_data_folder_ + "/Masks/retro_cine_edge_epi");
	Gadgetron::subtract(ref, edge_epi, diff);
	T s = Gadgetron::nrm2(diff);
	norm_ref = Gadgetron::nrm2(ref);
	std::cout << s / norm_ref << std::endl;
	EXPECT_LE(s / norm_ref, 0.15);
}
