# frozen_string_literal: true

RSpec.describe Numo::Liblinear do
  describe 'constant values' do
    it 'has a version number' do
      expect(Numo::Liblinear::VERSION).not_to be nil
    end

    it 'has solver type values', aggregate_failures: true do
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
  end

  describe 'classification' do
    let(:dataset) { Marshal.load(File.read(__dir__ + '/../iris.dat')) }
    let(:x) { dataset[0] }
    let(:y) { dataset[1] }
    let(:x_test) { dataset[2] }
    let(:y_test) { dataset[3] }
    let(:classes) { Numo::Int32[*y.to_a.uniq] }
    let(:n_classes) { classes.size }
    let(:n_samples) { x_test.shape[0] }
    let(:svc_param) { { solver_type: Numo::Liblinear::SolverType::L2R_L2LOSS_SVC_DUAL, C: 1 } }
    let(:svc_model) { Numo::Liblinear.train(x, y, svc_param) }
    let(:logit_param) { { solver_type: Numo::Liblinear::SolverType::L2R_LR_DUAL, C: 1 } }
    let(:logit_model) { Numo::Liblinear.train(x, y, logit_param) }

    it 'predicts labels with C-SVC', aggregate_failures: true do
      pr = Numo::Liblinear.predict(x_test, svc_param, svc_model)
      expect(pr.class).to eq(Numo::DFloat)
      expect(pr.shape[0]).to eq(n_samples)
      expect(pr.shape[1]).to be_nil
      expect(accuracy(y_test, pr)).to be_within(0.05).of(0.95)
    end

    it 'calculates decision function with C-SVC', aggregate_failures: true do
      df = Numo::Liblinear.decision_function(x_test, svc_param, svc_model)
      pr = Numo::Int32[*(Array.new(n_samples) { |n| classes[df[n, true].max_index] })]
      expect(df.class).to eq(Numo::DFloat)
      expect(df.shape[0]).to eq(n_samples)
      expect(df.shape[1]).to eq(n_classes)
      expect(accuracy(y_test, pr)).to be_within(0.05).of(0.95)
    end

    it 'predicts probabilities with Logistic Regression', aggregate_failures: true do
      pb = Numo::Liblinear.predict_proba(x_test, logit_param, logit_model)
      pr = Numo::Int32[*(Array.new(n_samples) { |n| classes[pb[n, true].max_index] })]
      expect(pb.class).to eq(Numo::DFloat)
      expect(pb.shape[0]).to eq(n_samples)
      expect(pb.shape[1]).to eq(n_classes)
      expect(accuracy(y_test, pr)).to be_within(0.05).of(0.95)
    end

    it 'performs 5-cross validation with C-SVC' do
      pr = Numo::Liblinear.cv(x, y, svc_param, 5)
      expect(accuracy(y, pr)).to be_within(0.05).of(0.85)
    end
  end

  describe 'regression' do
    let(:dataset) { Marshal.load(File.read(__dir__ + '/../housing.dat')) }
    let(:x) { dataset[0] }
    let(:y) { dataset[1] }
    let(:x_test) { dataset[2] }
    let(:y_test) { dataset[3] }
    let(:n_samples) { x_test.shape[0] }
    let(:svr_param) { { solver_type: Numo::Liblinear::SolverType::L2R_L2LOSS_SVR_DUAL, C: 0.01, p: 3.0 } }
    let(:svr_model) { Numo::Liblinear.train(x, y, svr_param) }

    it 'predicts target values with SVR', aggregate_failures: true do
      pr = Numo::Liblinear.predict(x_test, svr_param, svr_model)
      expect(pr.class).to eq(Numo::DFloat)
      expect(pr.shape[0]).to eq(n_samples)
      expect(pr.shape[1]).to be_nil
      expect(r2_score(y_test, pr)).to be >= 0.5
    end

    it 'calculates decision function with SVR', aggregate_failures: true do
      df = Numo::Liblinear.decision_function(x_test, svr_param, svr_model)
      pr = Numo::Liblinear.predict(x_test, svr_param, svr_model)
      err = (df - pr).abs.mean
      expect(df.class).to eq(Numo::DFloat)
      expect(df.shape[0]).to eq(n_samples)
      expect(df.shape[1]).to be_nil
      expect(err).to be <= 1e-8
    end

    it 'performs 5-cross validation with SVR' do
      pr = Numo::Liblinear.cv(x, y, svr_param, 5)
      expect(r2_score(y, pr)).to be >= 0.1
    end
  end
end
