#pragma once

namespace llmvis {

enum class ExperimentType {
    CHANGE_ATTENTION_WEIGHTS,
    MODIFY_LAYER_SIZES,
    ALTER_ACTIVATION_FUNCTIONS,
    INJECT_KNOWLEDGE,
    TEST_ROBUSTNESS
};

} // namespace llmvis 