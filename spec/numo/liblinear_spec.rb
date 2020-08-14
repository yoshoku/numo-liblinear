# frozen_string_literal: true

RSpec.describe Numo::Liblinear do
  describe 'constant values' do
    it 'has a version number' do
      expect(Numo::Liblinear::VERSION).not_to be nil
      expect(Numo::Liblinear::LIBLINEAR_VERSION).not_to be nil
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
      expect(Numo::Liblinear::SolverType::ONECLASS_SVM).to eq(21)
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
    let(:svc_param) { { solver_type: Numo::Liblinear::SolverType::L2R_L2LOSS_SVC_DUAL, C: 1, random_seed: 1 } }
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

    context 'when given training data  that contain all zero value feature' do
      let(:n_train_samples) { dataset[0].shape[0] }
      let(:n_test_samples) { dataset[2].shape[0] }
      let(:x) { Numo::NArray.hstack([dataset[0], Numo::DFloat.zeros(n_train_samples).expand_dims(1)]) }
      let(:x_test) { Numo::NArray.hstack([dataset[2], Numo::DFloat.new(n_test_samples).rand.expand_dims(1)]) }

      it 'predicts labels with C-SVC', aggregate_failures: true do
        pr = Numo::Liblinear.predict(x_test, svc_param, svc_model)
        expect(pr.class).to eq(Numo::DFloat)
        expect(pr.shape[0]).to eq(n_samples)
        expect(pr.shape[1]).to be_nil
        expect(accuracy(y_test, pr)).to be_within(0.05).of(0.95)
      end
    end
  end

  describe 'regression' do
    let(:dataset) { Marshal.load(File.read(__dir__ + '/../housing.dat')) }
    let(:x) { dataset[0] }
    let(:y) { dataset[1] }
    let(:x_test) { dataset[2] }
    let(:y_test) { dataset[3] }
    let(:n_samples) { x_test.shape[0] }
    let(:svr_param) { { solver_type: Numo::Liblinear::SolverType::L2R_L2LOSS_SVR_DUAL, C: 0.01, p: 3.0, random_seed: 1 } }
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

  describe 'outlier detection' do
    let(:dataset) { Marshal.load(File.read(__dir__ + '/../iris.dat')) }
    let(:pos_id) { dataset[1].eq(1).where }
    let(:neg_id) { dataset[1].eq(3).where }
    let(:x_pos) { dataset[0][pos_id, true] }
    let(:x_neg) { dataset[0][neg_id, true] }
    let(:n_pos_samples) { x_pos.shape[0] }
    let(:n_neg_samples) { x_neg.shape[0] }
    let(:y_pos) { Numo::Int32.cast([ 1] * n_pos_samples) }
    let(:y_neg) { Numo::Int32.cast([-1] * n_neg_samples) }
    let(:oc_svm_param) { { solver_type: Numo::Liblinear::SolverType::ONECLASS_SVM, C: 1, nu: 0.1, random_seed: 1 } }
    let(:oc_svm_model) { Numo::Liblinear.train(x_pos, y_pos, oc_svm_param) }

    it 'calculates decision function with one-class SVM' do
      df = Numo::Liblinear.decision_function(x_neg, oc_svm_param, oc_svm_model)
      expect(df.class).to eq(Numo::DFloat)
      expect(df.shape[0]).to eq(n_neg_samples)
      expect(df.shape[1]).to be_nil
      expect(df.lt(0.0).count).to eq(n_neg_samples)
    end

    it 'predicts labels with one-class SVM' do
      pr = Numo::Liblinear.predict(x_neg, oc_svm_param, oc_svm_model)
      expect(pr.class).to eq(Numo::DFloat)
      expect(pr.shape[0]).to eq(n_neg_samples)
      expect(pr.shape[1]).to be_nil
      expect(accuracy(y_neg, pr)).to be >= 0.9
    end
  end

  describe 'errors' do
    let(:dataset) { Marshal.load(File.read(__dir__ + '/../iris.dat')) }
    let(:x) { dataset[0] }
    let(:y) { dataset[1] }
    let(:svm_param) { { solver_type: Numo::Liblinear::SolverType::L2R_L2LOSS_SVC_DUAL, C: 1 } }
    let(:svm_model) { Numo::Liblinear.train(x, y, svm_param) }

    describe '#train' do
      it 'raises ArgumentError when given non two-dimensional array as sample array' do
        expect { described_class.train(Numo::DFloat.new(3, 2, 2).rand, Numo::DFloat.new(3).rand, svm_param) }.to raise_error(ArgumentError, 'Expect samples to be 2-D array.')
      end

      it 'raises ArgumentError when given non one-dimensional array as label array' do
        expect { described_class.train(Numo::DFloat.new(3, 2).rand, Numo::DFloat.new(3, 2).rand, svm_param) }.to raise_error(ArgumentError, 'Expect label or target values to be 1-D arrray.')
      end

      it 'raises ArgumentError when the number of smaples of sample array and label array are different' do
        expect { described_class.train(Numo::DFloat.new(5, 2).rand, Numo::DFloat.new(3).rand, svm_param) }.to raise_error(ArgumentError, 'Expect to have the same number of samples for samples and labels.')
        expect { described_class.train(Numo::DFloat.new(3, 2).rand, Numo::DFloat.new(5).rand, svm_param) }.to raise_error(ArgumentError, 'Expect to have the same number of samples for samples and labels.')
      end

      it 'raises ArgumentError when given invalid parameter value for libsvm' do
        svm_param[:C] = -100
        expect { described_class.train(Numo::DFloat.new(3, 2).rand, Numo::DFloat.new(3).rand, svm_param) }.to raise_error(ArgumentError, 'Invalid LIBLINEAR parameter is given: C <= 0')
      end
    end

    describe '#cv' do
      it 'raises ArgumentError when given non two-dimensional array as sample array' do
        expect { described_class.cv(Numo::DFloat.new(3, 2, 2).rand, Numo::DFloat.new(3).rand, svm_param, 5) }.to raise_error(ArgumentError, 'Expect samples to be 2-D array.')
      end

      it 'raises ArgumentError when given non one-dimensional array as label array' do
        expect { described_class.cv(Numo::DFloat.new(3, 2).rand, Numo::DFloat.new(3, 2).rand, svm_param, 5) }.to raise_error(ArgumentError, 'Expect label or target values to be 1-D arrray.')
      end

      it 'raises ArgumentError when the number of smaples of sample array and label array are different' do
        expect { described_class.cv(Numo::DFloat.new(5, 2).rand, Numo::DFloat.new(3).rand, svm_param, 5) }.to raise_error(ArgumentError, 'Expect to have the same number of samples for samples and labels.')
        expect { described_class.cv(Numo::DFloat.new(3, 2).rand, Numo::DFloat.new(5).rand, svm_param, 5) }.to raise_error(ArgumentError, 'Expect to have the same number of samples for samples and labels.')
      end

      it 'raises ArgumentError when given invalid parameter value for libsvm' do
        svm_param[:C] = -100
        expect { described_class.cv(Numo::DFloat.new(3, 2).rand, Numo::DFloat.new(3).rand, svm_param, 5) }.to raise_error(ArgumentError, 'Invalid LIBLINEAR parameter is given: C <= 0')
      end
    end

    describe '#predict' do
      it 'raises ArgumentError when given non two-dimensional array as sample array' do
        expect { described_class.predict(Numo::DFloat.new(3, 2, 2).rand, svm_param, svm_model) }.to raise_error(ArgumentError, 'Expect samples to be 2-D array.')
      end
    end

    describe '#decision_function' do
      it 'raises ArgumentError when given non two-dimensional array as sample array' do
        expect { described_class.decision_function(Numo::DFloat.new(3, 2, 2).rand, svm_param, svm_model) }.to raise_error(ArgumentError, 'Expect samples to be 2-D array.')
      end
    end

    describe '#predict_proba' do
      it 'raises ArgumentError when given non two-dimensional array as sample array' do
        expect { described_class.predict_proba(Numo::DFloat.new(3, 2, 2).rand, svm_param, svm_model) }.to raise_error(ArgumentError, 'Expect samples to be 2-D array.')
      end
    end

    describe '#load_model' do
      it 'raises IOError when failed load file' do
        expect { described_class.load_model('foo') }.to raise_error(IOError, "Failed to load file 'foo'")
      end
    end

    describe '#save_model' do
      it 'raises IOError when failed save file' do
        expect { described_class.save_model('', svm_param, svm_model) }.to raise_error(IOError, "Failed to save file ''")
      end
    end
  end
end
