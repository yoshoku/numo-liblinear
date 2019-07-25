# frozen_string_literal: true

RSpec.describe Numo::Liblinear do
  let(:iris) { Marshal.load(File.read(__dir__ + '/../iris.dat')) }
  let(:x) { iris[0] }
  let(:y) { iris[1] }
  let(:x_test) { iris[2] }
  let(:y_test) { iris[3] }
  let(:classes) { Numo::Int32[*y.to_a.uniq] }
  let(:n_classes) { classes.size }
  let(:n_samples) { x_test.shape[0] }
  let(:svc_param) { { solver_type: Numo::Liblinear::SolverType::L2R_L2LOSS_SVC_DUAL, C: 10 } }
  let(:svc_model) { Numo::Liblinear.train(x, y, svc_param) }
  let(:logit_param) { { solver_type: Numo::Liblinear::SolverType::L2R_LR_DUAL, C: 10 } }
  let(:logit_model) { Numo::Liblinear.train(x, y, logit_param) }

  it 'has a version number' do
    expect(Numo::Liblinear::VERSION).not_to be nil
  end

  it 'has some constant values', aggregate_failures: true do
    expect(Numo::Liblinear::SolverType::L2R_LR).to eq(0)
    expect(Numo::Liblinear::SolverType::L2R_L2LOSS_SVC_DUAL).to eq(1)
    expect(Numo::Liblinear::SolverType::L2R_L2LOSS_SVC).to eq(2)
    expect(Numo::Liblinear::SolverType::L2R_L1LOSS_SVC_DUAL).to eq(3)
    expect(Numo::Liblinear::SolverType::MCSVM_CS).to eq(4)
    expect(Numo::Liblinear::SolverType::L1R_L2LOSS_SVC).to eq(5)
    expect(Numo::Liblinear::SolverType::L1R_LR).to eq(6)
    expect(Numo::Liblinear::SolverType::L2R_LR_DUAL).to eq(7)
    expect(Numo::Liblinear::SolverType::L2R_L2LOSS_SVR).to eq(11)
    expect(Numo::Liblinear::SolverType::L2R_L2LOSS_SVR_DUAL).to eq(12)
    expect(Numo::Liblinear::SolverType::L2R_L1LOSS_SVR_DUAL).to eq(13)
  end

  it 'calculates decision function with C-SVC', aggregate_failures: true do
    func_vals = Numo::Liblinear.decision_function(x_test, svc_param, svc_model)
    predicted = Numo::Int32[*(Array.new(n_samples) { |n| classes[func_vals[n, true].max_index] })]
    accuracy = predicted.eq(y_test).count.fdiv(n_samples)
    expect(func_vals.class).to eq(Numo::DFloat)
    expect(func_vals.shape[0]).to eq(n_samples)
    expect(func_vals.shape[1]).to eq(n_classes)
    expect(accuracy).to be_within(0.05).of(0.95)
  end

  it 'predicts labels with C-SVC', aggregate_failures: true do
    predicted = Numo::Liblinear.predict(x_test, svc_param, svc_model)
    accuracy = predicted.eq(y_test).count.fdiv(n_samples)
    expect(predicted.class).to eq(Numo::DFloat)
    expect(predicted.shape[0]).to eq(n_samples)
    expect(predicted.shape[1]).to be_nil
    expect(accuracy).to be_within(0.05).of(0.95)
  end

  it 'predicts probabilities with Logistic Regression', aggregate_failures: true do
    probs = Numo::Liblinear.predict_proba(x_test, logit_param, logit_model)
    predicted = Numo::Int32[*(Array.new(n_samples) { |n| classes[probs[n, true].max_index] })]
    accuracy = predicted.eq(y_test).count.fdiv(n_samples)
    expect(probs.class).to eq(Numo::DFloat)
    expect(probs.shape[0]).to eq(n_samples)
    expect(probs.shape[1]).to eq(n_classes)
    expect(accuracy).to be_within(0.05).of(0.95)
  end
end
