#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

/*-
 * Copyright (c) 2009 Hans Petter Selasky. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <dev/usb/usb.h>

#include <dev/usb/usb_core.h>
#include <dev/usb/usb_busdma.h>
#include <dev/usb/usb_process.h>
#include <dev/usb/usb_sw_transfer.h>
#include <dev/usb/usb_util.h>

#include <dev/usb/usb_controller.h>
#include <dev/usb/usb_bus.h>
#include <dev/usb/controller/atmegadci.h>

#include <sys/rman.h>

static device_probe_t atmegadci_probe;
static device_attach_t atmegadci_attach;
static device_detach_t atmegadci_detach;
static device_shutdown_t atmegadci_shutdown;

struct atmegadci_super_softc {
	struct atmegadci_softc sc_otg;	/* must be first */
};

static void
atmegadci_clocks_on(struct usb2_bus *bus)
{
	/* TODO */
}

static void
atmegadci_clocks_off(struct usb2_bus *bus)
{
	/* TODO */
}

static int
atmegadci_probe(device_t dev)
{
	device_set_desc(dev, "ATMEL OTG integrated USB controller");
	return (0);
}

static int
atmegadci_attach(device_t dev)
{
	struct atmegadci_super_softc *sc = device_get_softc(dev);
	int err;
	int rid;

	/* setup MUSB OTG USB controller interface softc */
	sc->sc_otg.sc_clocks_on = &atmegadci_clocks_on;
	sc->sc_otg.sc_clocks_off = &atmegadci_clocks_off;

	/* initialise some bus fields */
	sc->sc_otg.sc_bus.parent = dev;
	sc->sc_otg.sc_bus.devices = sc->sc_otg.sc_devices;
	sc->sc_otg.sc_bus.devices_max = ATMEGA_MAX_DEVICES;

	/* get all DMA memory */
	if (usb2_bus_mem_alloc_all(&sc->sc_otg.sc_bus,
	    USB_GET_DMA_TAG(dev), NULL)) {
		return (ENOMEM);
	}
	rid = 0;
	sc->sc_otg.sc_io_res =
	    bus_alloc_resource_any(dev, SYS_RES_MEMORY, &rid, RF_ACTIVE);

	if (!(sc->sc_otg.sc_io_res)) {
		err = ENOMEM;
		goto error;
	}
	sc->sc_otg.sc_io_tag = rman_get_bustag(sc->sc_otg.sc_io_res);
	sc->sc_otg.sc_io_hdl = rman_get_bushandle(sc->sc_otg.sc_io_res);

	rid = 0;
	sc->sc_otg.sc_irq_res =
	    bus_alloc_resource_any(dev, SYS_RES_IRQ, &rid, RF_ACTIVE);
	if (!(sc->sc_otg.sc_irq_res)) {
		goto error;
	}
	sc->sc_otg.sc_bus.bdev = device_add_child(dev, "usbus", -1);
	if (!(sc->sc_otg.sc_bus.bdev)) {
		goto error;
	}
	device_set_ivars(sc->sc_otg.sc_bus.bdev, &sc->sc_otg.sc_bus);

	err = bus_setup_intr(dev, sc->sc_otg.sc_irq_res, INTR_TYPE_BIO | INTR_MPSAFE,
	    NULL, (void *)atmegadci_interrupt, sc, &sc->sc_otg.sc_intr_hdl);
	if (err) {
		sc->sc_otg.sc_intr_hdl = NULL;
		goto error;
	}
	err = atmegadci_init(&sc->sc_otg);
	if (!err) {
		err = device_probe_and_attach(sc->sc_otg.sc_bus.bdev);
	}
	if (err) {
		goto error;
	}
	return (0);

error:
	atmegadci_detach(dev);
	return (ENXIO);
}

static int
atmegadci_detach(device_t dev)
{
	struct atmegadci_super_softc *sc = device_get_softc(dev);
	device_t bdev;
	int err;

	if (sc->sc_otg.sc_bus.bdev) {
		bdev = sc->sc_otg.sc_bus.bdev;
		device_detach(bdev);
		device_delete_child(dev, bdev);
	}
	/* during module unload there are lots of children leftover */
	device_delete_all_children(dev);

	if (sc->sc_otg.sc_irq_res && sc->sc_otg.sc_intr_hdl) {
		/*
		 * only call atmegadci_uninit() after atmegadci_init()
		 */
		atmegadci_uninit(&sc->sc_otg);

		err = bus_teardown_intr(dev, sc->sc_otg.sc_irq_res,
		    sc->sc_otg.sc_intr_hdl);
		sc->sc_otg.sc_intr_hdl = NULL;
	}
	/* free IRQ channel, if any */
	if (sc->sc_otg.sc_irq_res) {
		bus_release_resource(dev, SYS_RES_IRQ, 0,
		    sc->sc_otg.sc_irq_res);
		sc->sc_otg.sc_irq_res = NULL;
	}
	/* free memory resource, if any */
	if (sc->sc_otg.sc_io_res) {
		bus_release_resource(dev, SYS_RES_MEMORY, 0,
		    sc->sc_otg.sc_io_res);
		sc->sc_otg.sc_io_res = NULL;
	}
	usb2_bus_mem_free_all(&sc->sc_otg.sc_bus, NULL);

	return (0);
}

static int
atmegadci_shutdown(device_t dev)
{
	struct atmegadci_super_softc *sc = device_get_softc(dev);
	int err;

	err = bus_generic_shutdown(dev);
	if (err)
		return (err);

	atmegadci_uninit(&sc->sc_otg);

	return (0);
}

static device_method_t atmegadci_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe, atmegadci_probe),
	DEVMETHOD(device_attach, atmegadci_attach),
	DEVMETHOD(device_detach, atmegadci_detach),
	DEVMETHOD(device_shutdown, atmegadci_shutdown),

	/* Bus interface */
	DEVMETHOD(bus_print_child, bus_generic_print_child),

	{0, 0}
};

static driver_t atmegadci_driver = {
	"atmegadci",
	atmegadci_methods,
	sizeof(struct atmegadci_super_softc),
};

static devclass_t atmegadci_devclass;

DRIVER_MODULE(atmegadci, atmelarm, atmegadci_driver, atmegadci_devclass, 0, 0);
MODULE_DEPEND(atmegadci, usb, 1, 1, 1);
