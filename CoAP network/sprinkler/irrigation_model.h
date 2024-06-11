#include <eml_trees.h>
    

static const EmlTreesNode irrigation_model_nodes[1] = {
  { 1, 495.000000f, -1, -2 } 
};

static const int32_t irrigation_model_tree_roots[1] = { 0 };

static const uint8_t irrigation_model_leaves[2] = { 0, 1 };

EmlTrees irrigation_model = {
        1,
        (EmlTreesNode *)(irrigation_model_nodes),	  
        1,
        (int32_t *)(irrigation_model_tree_roots),
        2,
        (uint8_t *)(irrigation_model_leaves),
        0,
        3,
        2,
    };

static inline int32_t irrigation_model_tree_0(const float *features, int32_t features_length) {
          if (features[1] < 495.000000f) {
              return 0;
          } else {
              return 1;
          }
        }
        

int32_t irrigation_model_predict(const float *features, int32_t features_length) {

        int32_t votes[2] = {0,};
        int32_t _class = -1;

        _class = irrigation_model_tree_0(features, features_length); votes[_class] += 1;
    
        int32_t most_voted_class = -1;
        int32_t most_voted_votes = 0;
        for (int32_t i=0; i<2; i++) {

            if (votes[i] > most_voted_votes) {
                most_voted_class = i;
                most_voted_votes = votes[i];
            }
        }
        return most_voted_class;
    }
    