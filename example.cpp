/// this is what the user will have to do:
class MyController : public BaseController
{
public:
    MyController() {registerForRegion(myRegion);}
    virtual void update(Region &r) override {
        // modify region
    }
};

class MyViewer : public BaseViewer
{
public:
    MyViewer() {registerForRegion(myRegion);}
    virtual void update(const Region &r) override {
        // read data from region
    }
};

/// this is the pseudo-code for the interfaces:
class BaseController {
public:
	void registerForRegion(Region &r) {
        // send registration for region via hpx to steerer for every X timesteps
    }
    void sendUpdateForRegion(Region &r) {
        // send updated region via hpx to steerer
    }
    void notifyTimeStepsOver(Region &r) { // gets called via hpx by LGDSteerer every X timesteps
        update(r);
        sendUpdateForRegion(r);
    }
    virtual void update(Region &r) = 0;
};

class LGDSteerer {
public:
    void receiveRegistration(Region &r, Controller &controller) { // receive via hpx
        controllers.insert(ControllerWithRegion(controller, r)); // insert into list of controllers-to-update
    }
    void receiveUpdatedRegion(Region &r, Controller &controller) { // receive via hpx
        queue.insert(r); // queue the region for update
    }
    void update(Region &r) { // update() method which gets called by main simulation
        for(ControllerWithRegion &controller : controllers) {
            notifyControllerIfInRegion(controller, r); // send notification via hpx to controllers if controllers are in the currently updated region
        }
        for(Region &r : queue) {
            applyUpdate(r);
        }
    }

};

class BaseViewer {
public:
    void registerForRegion(Region &r) {
        // send registration for region via hpx to writer for every X timesteps
    }
    void notifyTimeStepsOver(const Region &r) { // gets called via hpx by LGDSteerer every X timesteps
        update(r);
    }
    virtual void update(const Region &r) = 0;
};

class LGDWriter {
public:
    void receiveRegistration(Region &r, Viewer &viewer) { // receive via hpx
        viewers.insert(ViewerWithRegion(viewer, r)); // insert into list of viewers-to-update
    }
    void update(Region &r) { // update() method which gets called by main simulation
        for(ViewerWithRegion &viewer : viewers) {
            notifyViewerIfInRegion(viewer, r); // send notification via hpx if viewers if in currently updated region
        }
    }
};
