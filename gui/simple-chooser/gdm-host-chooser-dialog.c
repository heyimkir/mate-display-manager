/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2007 William Jon McCann <mccann@jhu.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gdm-host-chooser-dialog.h"
#include "gdm-host-chooser-widget.h"

#define GDM_HOST_CHOOSER_DIALOG_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GDM_TYPE_HOST_CHOOSER_DIALOG, GdmHostChooserDialogPrivate))

struct GdmHostChooserDialogPrivate
{
        GtkWidget *chooser_widget;
        int        kind_mask;
};

enum {
        PROP_0,
        PROP_KIND_MASK,
};

static void     gdm_host_chooser_dialog_class_init  (GdmHostChooserDialogClass *klass);
static void     gdm_host_chooser_dialog_init        (GdmHostChooserDialog      *host_chooser_dialog);
static void     gdm_host_chooser_dialog_finalize    (GObject                   *object);

G_DEFINE_TYPE (GdmHostChooserDialog, gdm_host_chooser_dialog, GTK_TYPE_DIALOG)

GdmChooserHost *
gdm_host_chooser_dialog_get_host (GdmHostChooserDialog *dialog)
{
        GdmChooserHost *host;

        g_return_val_if_fail (GDM_IS_HOST_CHOOSER_DIALOG (dialog), NULL);

        host = gdm_host_chooser_widget_get_host (GDM_HOST_CHOOSER_WIDGET (dialog->priv->chooser_widget));

        return host;
}

static void
_gdm_host_chooser_dialog_set_kind_mask (GdmHostChooserDialog *dialog,
                                        int                   kind_mask)
{
        if (dialog->priv->kind_mask != kind_mask) {
                dialog->priv->kind_mask = kind_mask;
        }
}

static void
gdm_host_chooser_dialog_set_property (GObject        *object,
                                      guint           prop_id,
                                      const GValue   *value,
                                      GParamSpec     *pspec)
{
        GdmHostChooserDialog *self;

        self = GDM_HOST_CHOOSER_DIALOG (object);

        switch (prop_id) {
        case PROP_KIND_MASK:
                _gdm_host_chooser_dialog_set_kind_mask (self, g_value_get_int (value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}

static void
gdm_host_chooser_dialog_get_property (GObject        *object,
                                      guint           prop_id,
                                      GValue         *value,
                                      GParamSpec     *pspec)
{
        switch (prop_id) {
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}

static void
on_response (GdmHostChooserDialog *dialog,
             gint                  response_id)
{
        switch (response_id) {
        case GTK_RESPONSE_APPLY:
                gdm_host_chooser_widget_refresh (GDM_HOST_CHOOSER_WIDGET (dialog->priv->chooser_widget));
                g_signal_stop_emission_by_name (dialog, "response");
                break;
        default:
                break;
        }
}

static GObject *
gdm_host_chooser_dialog_constructor (GType                  type,
                                     guint                  n_construct_properties,
                                     GObjectConstructParam *construct_properties)
{
        GdmHostChooserDialog      *dialog;

        dialog = GDM_HOST_CHOOSER_DIALOG (G_OBJECT_CLASS (gdm_host_chooser_dialog_parent_class)->constructor (type,
                                                                                                                           n_construct_properties,
                                                                                                                           construct_properties));


        dialog->priv->chooser_widget = gdm_host_chooser_widget_new (dialog->priv->kind_mask);
        gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area (GTK_DIALOG (dialog))), dialog->priv->chooser_widget);
        gtk_container_set_border_width (GTK_CONTAINER (dialog->priv->chooser_widget), 5);

        gtk_dialog_add_buttons (GTK_DIALOG (dialog),
                                GTK_STOCK_REFRESH, GTK_RESPONSE_APPLY,
                                GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                GTK_STOCK_CONNECT, GTK_RESPONSE_OK,
                                NULL);

        gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER_ALWAYS);
        gtk_container_set_border_width (GTK_CONTAINER (dialog), 12);
        gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
        gtk_window_set_title (GTK_WINDOW (dialog), _("Select System"));
        gtk_window_set_icon_name (GTK_WINDOW (dialog), "computer");

        g_signal_connect (dialog,
                          "response",
                          G_CALLBACK (on_response),
                          dialog);

        gtk_widget_show_all (GTK_WIDGET (dialog));

        return G_OBJECT (dialog);
}

static void
gdm_host_chooser_dialog_dispose (GObject *object)
{
        g_debug ("Disposing host_chooser_dialog");

        G_OBJECT_CLASS (gdm_host_chooser_dialog_parent_class)->dispose (object);
}

static void
gdm_host_chooser_dialog_class_init (GdmHostChooserDialogClass *klass)
{
        GObjectClass   *object_class = G_OBJECT_CLASS (klass);

        object_class->get_property = gdm_host_chooser_dialog_get_property;
        object_class->set_property = gdm_host_chooser_dialog_set_property;
        object_class->constructor = gdm_host_chooser_dialog_constructor;
        object_class->dispose = gdm_host_chooser_dialog_dispose;
        object_class->finalize = gdm_host_chooser_dialog_finalize;

        g_object_class_install_property (object_class,
                                         PROP_KIND_MASK,
                                         g_param_spec_int ("kind-mask",
                                                           "kind mask",
                                                           "kind mask",
                                                           0,
                                                           G_MAXINT,
                                                           0,
                                                           G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

        g_type_class_add_private (klass, sizeof (GdmHostChooserDialogPrivate));
}

static void
gdm_host_chooser_dialog_init (GdmHostChooserDialog *dialog)
{
        dialog->priv = GDM_HOST_CHOOSER_DIALOG_GET_PRIVATE (dialog);
}

static void
gdm_host_chooser_dialog_finalize (GObject *object)
{
        GdmHostChooserDialog *host_chooser_dialog;

        g_return_if_fail (object != NULL);
        g_return_if_fail (GDM_IS_HOST_CHOOSER_DIALOG (object));

        host_chooser_dialog = GDM_HOST_CHOOSER_DIALOG (object);

        g_return_if_fail (host_chooser_dialog->priv != NULL);

        G_OBJECT_CLASS (gdm_host_chooser_dialog_parent_class)->finalize (object);
}

GtkWidget *
gdm_host_chooser_dialog_new (int kind_mask)
{
        GObject *object;

        object = g_object_new (GDM_TYPE_HOST_CHOOSER_DIALOG,
                               "kind-mask", kind_mask,
                               NULL);

        return GTK_WIDGET (object);
}
