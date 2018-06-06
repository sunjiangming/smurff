from scipy import sparse
import math
from sklearn.metrics import mean_squared_error

def calc_rmse(predictions):
    return math.sqrt(mean_squared_error([p.val for p in predictions], [p.pred_avg for p in predictions]))

class Prediction:
    """Stores predictions for a single point in the matrix/tensor

    Attributes
    ----------
    coords : shape
        Position of this prediction in the train matrix/tensor
    val :  float
        True value or "nan" if no true value is known
    nsamples : int
        Number of samples collected to make this prediction
    pred_1sample :  float
        Predicted value using only the last sample
    pred_avg :  float
        Predicted value using the average prediction across all samples
    var : float
        Variance amongst predictions across all samples
    pred_all : list
        List of predictions, one for each sample

    """
    @staticmethod
    def fromTestMatrix(test_matrix):
        """Creates a list of predictions from a scipy sparse matrix"

        Parameters
        ----------
        test_matrix : scipy sparse matrix

        Returns
        -------
        list
            List of :class:`Prediction`. Only the coordinate and true value is filled.

        """

        return [ Prediction((i, j), v) for i,j,v in zip(*sparse.find(test_matrix)) ]
    
    def __init__(self, coords, val,  pred_1sample = float("nan"), pred_avg = float("nan"), var = float("nan"), nsamples = -1):
        self.coords = coords
        self.nsamples = nsamples
        self.val = val
        self.pred_1sample = pred_1sample
        self.pred_avg = pred_avg
        self.pred_all = []
        self.var = var

    def average(self, pred):
        self.nsamples += 1
        if self.nsamples == 0:
            self.pred_avg = pred
            self.var = 0
            self.pred_1sample = pred
        else:
            delta = pred - self.pred_avg
            self.pred_avg = (self.pred_avg + delta / (self.nsamples + 1))
            self.var = self.var + delta * (pred - self.pred_avg)
            self.pred_1sample = pred
    
    def add_sample(self, pred):
        self.average(pred)
        self.pred_all.append(pred)
            
    def __str__(self):
        return "%s: %.2f | 1sample: %.2f | avg: %.2f | var: %.2f | all: %s " % (self.coords, self.val, self.pred_1sample, self.pred_avg, self.var, self.pred_all)

    def __repr__(self):
        return str(self)

    def __gt__(self, circle2):
        return self.coords > circle2.coords

