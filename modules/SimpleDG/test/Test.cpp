
#include <iostream>
#include <string>
#include <chrono>
#include <memory>

#include "sdg/DependencyGraph.h"

using namespace std::chrono;

struct SObject {

    SObject(const std::string& name): name(name) {}

    void print() const {
        std::cout << name << std::endl;
    }

    std::string name;
};

struct SResource {
    size_t id = 0;

    explicit SResource(const size_t inId): id(inId) {}

    friend COutputArchive& operator<<(COutputArchive& archive, const SResource& resource) {
        archive << resource.id;
        return archive;
    }
};

int main() {

    {
        TRWDependencyGraph<TShared<SObject>, TShared<SResource>, TKahnTopologicalSort> graph;

        const TShared<SResource> hdrColor{0};
        const TShared<SResource> depth{1};
        const TShared<SResource> history{2};

        // GBuffer writes HDR + depth
        size_t gbufferPass = graph.addNode(std::make_shared<SObject>("gbufferPass"));
        graph.addWrite(gbufferPass, hdrColor);
        graph.addWrite(gbufferPass, depth);

        // Lighting reads GBuffer HDR + depth, writes HDR (in-place lighting)
        size_t lightingPass = graph.addNode(std::make_shared<SObject>("lightingPass"));
        graph.addRead(lightingPass, hdrColor);
        graph.addRead(lightingPass, depth);
        graph.addWrite(lightingPass, hdrColor);

        // TAA reads current HDR + history, writes HDR
        size_t taaPass = graph.addNode(std::make_shared<SObject>("taaPass"));
        graph.addRead(taaPass, hdrColor);
        graph.addRead(taaPass, history);
        graph.addWrite(taaPass, hdrColor);

        // Bloom threshold reads HDR, writes HDR (destructive)
        size_t bloomThresholdPass = graph.addNode(std::make_shared<SObject>("bloomThresholdPass"));
        graph.addRead(bloomThresholdPass, hdrColor);
        graph.addWrite(bloomThresholdPass, hdrColor);

        // Upscale reads HDR, writes HDR
        size_t upscalePass = graph.addNode(std::make_shared<SObject>("upscalePass"));
        graph.addRead(upscalePass, hdrColor);
        graph.addWrite(upscalePass, hdrColor);

        // Post-process reads HDR, writes HDR
        size_t postProcessPass = graph.addNode(std::make_shared<SObject>("postProcessPass"));
        graph.addRead(postProcessPass, hdrColor);
        graph.addWrite(postProcessPass, hdrColor);

        // History resolve reads final HDR, writes history (feedback loop)
        size_t historyResolvePass = graph.addNode(std::make_shared<SObject>("historyResolvePass"));
        graph.addRead(historyResolvePass, hdrColor);
        graph.addWrite(historyResolvePass, history);

#ifdef USING_SIMPLESTL
        const auto order = graph.buildExecutionOrder();

        order.forEach([&](size_t, const size_t& node) {
            std::cout << graph.getNode(node)->name << " -> ";
        });
#else
        const auto order = graph.buildExecutionOrder();

        for (const auto& node : order) {
            std::cout << graph.getNode(node)->name << " -> ";
        }
#endif
        std::cout << std::endl << std::endl;

        /*
        Resource lifetime tracking,
        aliasing,
        barrier synthesis,
        pass culling,
        track resource usage,
        unused write culling,
        emit barriers,
        and split graphics
         */
    }

    return 0;
}