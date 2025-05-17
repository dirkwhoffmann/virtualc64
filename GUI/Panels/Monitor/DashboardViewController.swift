// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import SwiftUI

enum PanelType: Int {
    
    case Combined       = 0
    case CIAA           = 1
    case CIAB           = 2
    case AmigaMhz       = 3
    case AmigaFps       = 4
    case HostLoad       = 5
    case HostFps        = 6
    case AudioFillLevel = 7
    case WaveformL      = 8
    // case WaveformR      = 9
}

@MainActor
class OverviewController : NSViewController {
    
 
    @IBOutlet weak var ciaABox: NSBox!
    @IBOutlet weak var ciaBBox: NSBox!
    @IBOutlet weak var hostLoadBox: NSBox!
    @IBOutlet weak var hostFpsBox: NSBox!
    @IBOutlet weak var amigaFpsBox: NSBox!
    @IBOutlet weak var amigaMhzBox: NSBox!
    @IBOutlet weak var fillLevelBox: NSBox!
    @IBOutlet weak var waveformLBox: NSBox!
    // @IBOutlet weak var waveformRBox: NSBox!
}

class DashboardViewController: NSViewController {
    
    @IBOutlet weak var containerView: NSView!
    
    var emu: EmulatorProxy?
    var myController: MyController?
    
    var type: PanelType? {
        didSet {
            switchToPanel(type: type)
        }
    }
    
    var multiPanelController: NSViewController!
    var singlePanelController: NSViewController!
    
    let ciaAPanel = CIAAPanel(frame: NSRect.zero)
    let ciaBPanel = CIABPanel(frame: NSRect.zero)
    let amigaFpsPanel = C64FpsPanel(frame: NSRect.zero)
    let amigaMhzPanel = C64MhzPanel(frame: NSRect.zero)
    let hostLoadPanel = HostLoadPanel(frame: NSRect.zero)
    let hostFpsPanel = HostFpsPanel(frame: NSRect.zero)
    let fillLevelPanel = AudioFillLevelPanel(frame: NSRect.zero)
    let waveformLPanel = WaveformPanel(frame: NSRect.zero, channel: 0)
    // let waveformRPanel = WaveformPanel(frame: NSRect.zero, channel: 1)
    
    override func viewDidLoad() {

        super.viewDidLoad()
   
        // Get the storyboard from the resources bundle
        let storyboard = NSStoryboard(name: "Dashboard", bundle: nil)

        // Load the view controllers from the storyboard
        multiPanelController = storyboard.instantiateController(withIdentifier: "ViewController1") as? NSViewController
        singlePanelController = storyboard.instantiateController(withIdentifier: "ViewController2") as? NSViewController
    }
    
    func windowDidResize(_ notification: Notification) {
    
    }
    
    private func switchToPanel(type: PanelType?) {
        
        guard let type = type else { return }
        
        func add(_ subView: NSView, to parentView: NSView) {
            
            // Add padding to the SwiftUI view
            if let panel = subView as? DashboardPanel {
                if parentView is NSBox {
                    panel.padding = SwiftUICore.EdgeInsets(top: 4.0, leading: 4.0, bottom: 0.0, trailing: 4.0)
                } else {
                    // panel.padding = SwiftUICore.EdgeInsets(top: 20.0, leading: 20.0, bottom: 20.0, trailing: 20.0)
                    panel.padding = SwiftUICore.EdgeInsets(top: 8.0, leading: 8.0, bottom: 8.0, trailing: 8.0)
                }
            }

            // Add the view
            parentView.addSubview(subView)
            (parentView as? NSBox)?.isTransparent = true
                        
            // Let the added view span the entire area of the parent view
            subView.translatesAutoresizingMaskIntoConstraints = false
            NSLayoutConstraint.activate([
                subView.leadingAnchor.constraint(equalTo: parentView.leadingAnchor),
                subView.trailingAnchor.constraint(equalTo: parentView.trailingAnchor),
                subView.topAnchor.constraint(equalTo: parentView.topAnchor),
                subView.bottomAnchor.constraint(equalTo: parentView.bottomAnchor)
            ])
        }
        
        switch type {
            
        case .Combined:
            
            view.window?.minSize = .zero
            switchToViewController(controller: multiPanelController)
            view.window?.minSize = NSSize(width: 400, height: 600)
            
            if let controller = children.first as? OverviewController {
                
                add(ciaAPanel, to: controller.ciaABox)
                add(ciaBPanel, to: controller.ciaBBox)
                add(amigaMhzPanel, to: controller.amigaMhzBox)
                add(amigaFpsPanel, to: controller.amigaFpsBox)
                add(hostLoadPanel, to: controller.hostLoadBox)
                add(hostFpsPanel, to: controller.hostFpsBox)
                add(fillLevelPanel, to: controller.fillLevelBox)
                add(waveformLPanel, to: controller.waveformLBox)
                // add(waveformRPanel, to: controller.waveformRBox)
            }
        default:
            
            view.window?.minSize = .zero
            switchToViewController(controller: singlePanelController)
            view.window?.minSize = NSSize(width: 200, height: 160)

            if let view = children.first?.view {

                // Remove the old view
                for oldView in view.subviews { oldView.removeFromSuperview() }
                
                switch type {
                    
                case .CIAA: add(ciaAPanel, to: view)
                case .CIAB: add(ciaBPanel, to: view)
                case .AmigaMhz: add(amigaMhzPanel, to: view)
                case .AmigaFps: add(amigaFpsPanel, to: view)
                case .HostLoad: add(hostLoadPanel, to: view)
                case .HostFps: add(hostFpsPanel, to: view)
                case .AudioFillLevel: add(fillLevelPanel, to: view)
                case .WaveformL: add(waveformLPanel, to: view)
                // case .WaveformR: add(waveformRPanel, to: view)
                
                default:
                    fatalError()
                }
            }
        }
    }
    
    private func switchToViewController(controller newController: NSViewController) {
        
        if let currentController = children.first  {
            
            // Remove the current view controller and its view
            currentController.view.removeFromSuperview()
            currentController.removeFromParent()
                        
            // Add the new view controller and its view
            let newView = newController.view
            addChild(newController)
            containerView.addSubview(newView)
            
            // Resize the window
            view.window?.setContentSize(newView.frame.size)
            newController.view.frame = containerView.bounds
            newController.view.autoresizingMask = [.width, .height]
            
            // Make the new view span the entire area
            newController.view.translatesAutoresizingMaskIntoConstraints = false
            NSLayoutConstraint.activate([
                newController.view.topAnchor.constraint(equalTo: containerView.topAnchor),
                newController.view.bottomAnchor.constraint(equalTo: containerView.bottomAnchor),
                newController.view.leadingAnchor.constraint(equalTo: containerView.leadingAnchor),
                newController.view.trailingAnchor.constraint(equalTo: containerView.trailingAnchor)
            ])
        }
    }
    
    func continuousRefresh() {

        guard let emu = emu else { return }
        guard let myController = myController else { return }
        
        // Host
        let stats = emu.stats
        hostLoadPanel.model.add(stats.cpuLoad)
        hostFpsPanel.model.add(myController.speedometer.gpuFps)

        // Emulator
        amigaFpsPanel.model.add(myController.speedometer.emuFps)
        amigaMhzPanel.model.add(myController.speedometer.mhz)
        
        // CIAs
        ciaAPanel.model.add(1.0 - emu.cia1.stats.idlePercentage)
        ciaBPanel.model.add(1.0 - emu.cia2.stats.idlePercentage)

        // Audio
        fillLevelPanel.model.add(emu.audioPort.stats.fillLevel)
        waveformLPanel.update()
        // waveformRPanel.update()
    }
}
