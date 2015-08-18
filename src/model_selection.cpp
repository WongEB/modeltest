#include "model_selection.h"

#include <cassert>
#include <cmath>
#include <algorithm>
#include <iomanip>

namespace modeltest {

using namespace std;

static bool compare_score(const selection_model &m1, const selection_model &m2)
{
    return m1.score < m2.score;
}

ModelSelection::ModelSelection(const vector<Model *> &c_models,
                               ic_type type)
{
    double sum_exp = 0.0;

    models.resize(c_models.size());
    for (size_t i=0; i<c_models.size(); i++)
    {
        models[i].model = c_models[i];
        switch(type)
        {
        case ic_lnl:
            ic_name = "lnL";
            models[i].score = models[i].model->get_lnl();
            break;
        case ic_bic:
            ic_name = "BIC";
            models[i].score = models[i].model->get_bic();
            break;
        case ic_aic:
            ic_name = "AIC";
            models[i].score = models[i].model->get_aic();
            break;
        case ic_aicc:
            ic_name = "AICc";
            models[i].score = models[i].model->get_aicc();
            break;
        case ic_dt:
            ic_name = "DT";
            models[i].score = models[i].model->get_dt();
            break;
        }
    }

    sort(models.begin(), models.end(), compare_score);

    for (size_t i=0; i<models.size(); i++)
    {
        models[i].delta = models[i].score - models[0].score;
        sum_exp += exp(-0.5 * models[i].delta);
    }

    for (size_t i=0; i<models.size(); i++)
    {
        /* weights */
        models[i].weight = exp(-0.5 * models[i].delta) / sum_exp;
    }
}

size_t ModelSelection::size() const
{
    return models.size();
}

const selection_model & ModelSelection::get_model(size_t i) const
{
    assert (i < models.size());
    return models[i];
}

void ModelSelection::print(ostream &out, mt_size_t limit)
{
    mt_size_t n_models = limit;
    out << endl
        << setw(8) << left << ic_name << "  "
        << setw(15) << left << "model"
        << setw(15) << right << "lnL"
        << setw(15) << "score"
        << setw(15) << "delta"
        << setw(10) << "weight"
        << endl;
    out << setw(80) << setfill('-') << "" << setfill(' ') << endl;
    if (n_models<=0 || n_models>models.size())
        n_models = models.size();
    for (size_t i=0; i<n_models; i++)
    {
        out << setprecision(4)
            << setw(8) << right << i+1 << "  "
            << setw(15) << left << models[i].model->get_name()
            << setw(15) << right << models[i].model->get_lnl()
            << setw(15) << models[i].score
            << setw(15) << models[i].delta
            << setw(10) << models[i].weight
            << endl;
    }
    out << setw(80) << setfill('-') << "" << setfill(' ') << endl;
}

}
