/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utestONNXInferenceModel
#include "GaudiKernel/GaudiException.h"
#include "GaussinoONNX/ONNXModelServer.h"
#include <boost/test/unit_test.hpp>

using Server = Gsino::ML::ONNX::ModelServer;
using Value  = Gsino::ML::ONNX::Value;
using Tensor = Gsino::ML::ONNX::Tensor;

Server* generate_model_from_file( const std::string& model_name ) {
  auto* model = new Server{ model_name };
  model->setLevel( MSG::INFO );
  std::string loc = std::getenv( "GAUSSINOONNXROOT" );
  loc += "/models/" + model_name + ".onnx";
  model->setGraphOptimization( "ENABLE_ALL" );
  model->loadModel( loc );
  model->setupSession();
  return model;
}

bool compare( float output, float expected ) { return std::fabs( output - expected ) < 1e-5; }
bool compare( std::vector<float> output_v, std::vector<float> expected_v ) {
  return std::equal( output_v.begin(), output_v.end(), expected_v.begin(),
                     [&]( auto l, auto r ) { return compare( l, r ); } );
}

BOOST_AUTO_TEST_CASE( checkNumberOfInputs ) {
  auto  model = generate_model_from_file( "Sum2FloatInputsTo1FloatOutputModel" );
  float input = 1.1;
  BOOST_CHECK_THROW( model->evaluate<float>( input ), GaudiException );
  BOOST_CHECK( compare( model->evaluate<float>( input, input ), 2.2 ) );
  BOOST_CHECK_THROW( model->evaluate<float>( input, input, input ), GaudiException );
  delete model;
}

BOOST_AUTO_TEST_CASE( checkNumberOfOutputs ) {
  auto               model   = generate_model_from_file( "Sum2FloatInputsTo2FloatOutputsModel" );
  std::vector<float> input_0 = { 1.4 };
  std::vector<float> input_1 = { 5.0 };
  BOOST_CHECK_THROW( model->evaluate<std::vector<float>>( input_0, input_1 ), GaudiException );
  auto vector   = model->evaluate<std::vector<Tensor>>( input_0, input_1 );
  auto output_0 = vector[0].GetTensorData<float>()[0];
  auto output_1 = vector[1].GetTensorData<float>()[0];
  BOOST_CHECK( compare( output_0, 1.4 ) );
  BOOST_CHECK( compare( output_1, 5.0 ) );
  delete model;
}

BOOST_AUTO_TEST_CASE( checkTypeOfOutput ) {
  auto  model = generate_model_from_file( "Sum2FloatInputsTo1FloatOutputModel" );
  float input = 1.1;
  BOOST_CHECK( compare( model->evaluate<float>( input, input ), 2.2 ) );
  BOOST_CHECK( compare( model->evaluate<std::vector<float>>( input, input )[0], 2.2 ) );
  BOOST_CHECK_THROW( compare( model->evaluate<int>( input, input ), 2.2 ), GaudiException );
  BOOST_CHECK_THROW( compare( model->evaluate<double>( input, input ), 2.2 ), GaudiException );
  BOOST_CHECK_THROW( compare( model->evaluate<std::vector<int>>( input, input )[0], 2.2 ), GaudiException );
  BOOST_CHECK_THROW( compare( model->evaluate<std::vector<double>>( input, input )[0], 2.2 ), GaudiException );
  delete model;
}

BOOST_AUTO_TEST_CASE( checkTypeOfInputs ) {
  auto               model       = generate_model_from_file( "Sum2FloatInputsTo1FloatOutputModel" );
  float              input_ok_0  = 3.6;
  float              input_ok_1  = 4.0;
  int                input_bad_0 = 4;
  std::vector<float> input_ok_2{ 4.0 };
  std::vector<int>   input_bad_1{ 4 };

  BOOST_CHECK( compare( model->evaluate<float>( input_ok_0, input_ok_1 ), 7.6 ) );
  BOOST_CHECK( compare( model->evaluate<float>( input_ok_0, input_ok_2 ), 7.6 ) );
  BOOST_CHECK_THROW( compare( model->evaluate<float>( input_ok_0, input_bad_0 ), 7.6 ), GaudiException );
  BOOST_CHECK_THROW( compare( model->evaluate<float>( input_ok_0, input_bad_1 ), 7.6 ), GaudiException );
  delete model;
}

BOOST_AUTO_TEST_CASE( checkMultiDimInputs ) {
  auto                 model      = generate_model_from_file( "Multiply2TensorsModel3x3" );
  std::vector<int64_t> dims       = { 3, 3 };
  std::vector<float>   input_ok_0 = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  Tensor input_ok_1 = Tensor::CreateTensor<float>( model->getMemoryInfo(), input_ok_0.data(), input_ok_0.size(),
                                                   dims.data(), dims.size() );
  Tensor input_ok_2 = Tensor::CreateTensor<float>( model->getMemoryInfo(), input_ok_0.data(), input_ok_0.size(),
                                                   dims.data(), dims.size() );
  std::vector<float> output_ok_0 = { 30, 36, 42, 66, 81, 96, 102, 126, 150 };
  BOOST_CHECK( compare( model->evaluate<std::vector<float>>( input_ok_0, input_ok_0 ), output_ok_0 ) );
  // input: 2 tensors, output: 1 contiguous vector
  BOOST_CHECK( compare( model->evaluate<std::vector<float>>( input_ok_1, input_ok_2 ), output_ok_0 ) );
  // input: 2 contiguous vectors, output: 1 tensor
  auto fromTensor = model->evaluate<Tensor>( input_ok_0, input_ok_0 );
  BOOST_CHECK( compare( model->makeContiguous<float>( fromTensor ), output_ok_0 ) );
  // input: 2 contiguous vectors, output: 1 vector of tensors
  auto fromTensors = model->evaluate<std::vector<Tensor>>( input_ok_0, input_ok_0 );
  BOOST_CHECK( compare( model->makeContiguous<float>( fromTensors[0] ), output_ok_0 ) );
  delete model;
}
