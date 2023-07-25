import numpy as np
import tensorflow as tf

from model.GNN import GNN
from util.gnn_data import GnnLoader
from util.setting import logger


def early_stopping(
    rel_fn: int, best_fn: int, stopping_step: int, flag_step: int
) -> tuple:
    """Judge whether stop model training early."""
    if best_fn > rel_fn:
        best_fn = rel_fn
        stopping_step = 0
    else:
        stopping_step += 1

    if stopping_step == flag_step:
        logger.warn("Early stop is triggered")
        should_stop = True
    else:
        should_stop = False

    return best_fn, stopping_step, should_stop


def validation(
    sess: tf.Session, model: GNN, data_generator: GnnLoader, threshold: float
) -> dict:
    """Validating recommendation model with validating data."""
    # benign data (Note: positives are predicted as cyber threats)
    # new - added tp_m and fn_m
    tn_b, fp_b, tp_m, fn_m = 0, 0, 0, 0
    for i_batch in range(data_generator.n_batch_val):
        # batch_data, gt_val = data_generator.generate_val_batch(i_batch)
        batch_data = data_generator.generate_val_batch(i_batch)
        feed_dict = data_generator.generate_test_val_feed_dict(model, batch_data)
        batch_rel = model.eval(sess, feed_dict=feed_dict)
        batch_rel = np.diag(batch_rel)

        # our validation set does not include positive (malicious) samples,
        # so that the prediction result is either true negative or false positive
        prediction = batch_rel < threshold
        # # new - val set can now include positive samples
        # for idx, item in enumerate(prediction):
        #     if item == gt_val[idx]:
        #         # False pos/neg
        #         if item == 1:
        #             fn_m += 1
        #         else:
        #             fp_b += 1
        #     else:
        #         # True pos/neg
        #         if item == 1:
        #             tn_b += 1
        #         else:
        #             tp_m += 1

        # new - commented out old code

        batch_tn = np.sum(prediction)
        batch_fp = prediction.size - batch_tn
        tn_b += batch_tn
        fp_b += batch_fp

    # rel_stat = {"tn_b": tn_b, "fp_b": fp_b, "tp_m": tp_m, "fn_m": fn_m}
    rel_stat = {"tn_b": tn_b, "fp_b": fp_b}

    return rel_stat


def test(
    sess: tf.Session, model: GNN, data_generator: GnnLoader, threshold: float
) -> dict:
    """Evaluating recommendation model with testing data."""
    # benign data (Note: positives are predicted as cyber threats)
    # new - added tp_m and fn_m
    tn_b, fp_b, tp_m, fn_m = 0, 0, 0, 0
    # print("Checking n_batch_test", data_generator.n_batch_test)
    for i_batch in range(data_generator.n_batch_test):
        # batch_data, gt_test = data_generator.generate_test_batch(i_batch)
        batch_data = data_generator.generate_test_batch(i_batch)
        feed_dict = data_generator.generate_test_val_feed_dict(model, batch_data)
        batch_rel = model.eval(sess, feed_dict=feed_dict)
        batch_rel = np.diag(batch_rel)

        # our test set (in this implementation) does not include positive (malicious) samples,
        # so that the prediction result is either true negative or false positive

        # if (i_batch == 0):
        #   print("Feed dict:", feed_dict)
        # print("Entity embeddings:", model.e_e)
        #   print("Batch data printing:", batch_data)
        #  print('\n\n\n Feed dict:', feed_dict)
        # print('\n\n\n Batch rel:', batch_rel)

        prediction = batch_rel < threshold
        # # new - val set can now include positive samples
        # for idx, item in enumerate(prediction):
        #     if item == gt_test[idx]:
        #         # False pos/neg
        #         if item == 1:
        #             fn_m += 1
        #         else:
        #             fp_b += 1
        #     else:
        #         # True pos/neg
        #         if item == 1:
        #             tn_b += 1
        #         else:
        #             tp_m += 1

        # new - commented out old code

        batch_tn = np.sum(prediction)
        batch_fp = prediction.size - batch_tn
        tn_b += batch_tn
        fp_b += batch_fp

        # if (i_batch == 0):
        #   print('\n\n\n Prediction:', prediction)
        #  print('\n\n\n batch_tn:', batch_tn)
        # print('\n\n\n batch_fp:', batch_fp)

    # rel_stat = {"tn_b": tn_b, "fp_b": fp_b, "tp_m": tp_m, "fn_m": fn_m}
    rel_stat = {"tn_b": tn_b, "fp_b": fp_b}

    return rel_stat


def pred_inter(sess: tf.Session, model: GNN, inters: list) -> np.ndarray:
    """Predicting recommendation score for interactions

    Example: [[1,2], [1,3], [1,4]]
    """
    inter_size = np.size(inters, 0)

    e = np.zeros(shape=inter_size)
    inter_e = np.zeros(shape=inter_size)

    for idx, inter in enumerate(inters):
        e[idx] = inter[0]
        inter_e[idx] = inter[1]

    feed_dict = {
        model.e: e,
        model.neg_e: inter_e,
        # hardcode dropping probability
        model.mess_dropout: [0, 0, 0, 0, 0, 0],
    }

    inter_rel = model.eval(sess, feed_dict=feed_dict)
    inter_rel = np.diag(inter_rel)

    return inter_rel
